//black, blue, green, cyan, red, magenta, brown, grey, 
//dark grey, light blue, light green, light cyan, light red,
//light magenta, yellow, white
//this is an array of RGB values for CGA palette above
const cga_palette =
[
    0x00, 0x00, 0x00, //black 0
    0x00, 0x00, 0xA8, //blue 1
    0x00, 0xA8, 0x00, //green 2
    0x00, 0xA8, 0xA8, //cyan 3
    0xA8, 0x00, 0x00, //red 4
    0xA8, 0xA8, 0x00, //magenta 5
    0xA8, 0x54, 0x00, //brown 6
    0xA8, 0xA8, 0xA8, //grey 7
    0x54, 0x54, 0x54, //dark grey 8
    0x54, 0x54, 0xFC, //light blue 9
    0x54, 0xFC, 0x54, //light green 10
    0x54, 0xFC, 0xFC, //light cyan 11
    0xFC, 0x54, 0x54, //light red 12
    0xFC, 0x54, 0xFC, //light magenta 13
    0xFC, 0xFC, 0x54, //yellow 14
    0xFC, 0xFC, 0xFC //white 15
];

//number of tiles in the scene
const lvl_0_tile_size = 7;
//the tiles xy position
const lvl_0_tile_xy = 
[
    64, 110,
    96, 125,
    128, 130,
    165, 140,
    192, 135,
    236, 140,
    280, 155
];

var lvl_0_tile_inc_count = 0;
//next platform set (8 bits total, 1 bit per platform)
//0 = -platform_move_speed
//1 = platform_move_speed
const lvl_0_tile_inc = 
[
    //only 7 platforms, hence the pattern
    '0000101', 1,
    '0001001', 0,
    '0110000', 1
];

var main_sprite;
//arr of platforms
var platform = [];
//bool for tracking player_jump
var player_jump = false;
//for tracking original jump position
var main_sprite_jump_y = 0;
//for testing against new y (used in conjunction with the jumping)
var main_sprite_old_y = 0;
const main_sprite_max_jump_height = 20;

let platform_update_count = 1;
const platform_update_treat_num = 1;
let platform_update_flag = false;

//track how many treats you have acquired
var treat_collects = 0;
//track how many lives we have left
var hearts_left = 3;
//how many pixels we move when the platforms move
var platform_move_px = 12;
var platform_move_speed = 0.03;
//whether the platform is moving by the move_px above
var platform_moving = false;
//this flag disables if we are falling which also disables double jumping which is good.
let can_jump = true;

var keys = 
{
    space: false,
    left: false,
    right: false
};

//sounds
var sound_jump;
var sound_jump_land;
var sound_death;
var sound_get_treat;
var sound_get_treat2;
var sound_player_hit;
var sound_player_respawn;

window.onkeydown = function(e) 
{
    let kc = e.keyCode;
    e.preventDefault();
    if      (kc === 65) keys.left = true;  // only one key per event
    else if (kc === 68) keys.right = true;
    else if (kc === 32) keys.space = true;
};

window.onkeyup = function(e) 
{
    let kc = e.keyCode;
    e.preventDefault();
    if      (kc === 65) keys.left = false;  // only one key per event
    else if (kc === 68) keys.right = false;
    else if (kc === 32) keys.space = false;
};

let platform_moving_arr = [];
let platform_moving_dir = 0;
function platform_move_update()
{
    if (platform_moving)
    {
        let platforms_left = 0;
        //+=2 = old player y is stored in elem 1 and platform id is store in elem 2
        for (let i = 0; i < platform_moving_arr.length; i+=2)
        {
            //0 = old player y
            //1 = y id
            if (platform_moving_arr[i+1] != -1)
            {
                //if they are moving down, then use the old player y to check whether
                //we have moved enough, then disable the movement
                if (platform_moving_dir == platform_move_speed)
                {
                    if (platform[platform_moving_arr[i+1]].y <= (platform_moving_arr[i] + platform_move_px))
                        platform[platform_moving_arr[i+1]].y += platform_move_px * platform_moving_dir;
                    else
                        //reset ids so we dont use them
                        platform_moving_arr[i+1] = -1;
                }

                //if they are moving up, then use the old player y to check whether
                //we have moved enough, then disable the movement
                else if (platform_moving_dir == -platform_move_speed)
                {
                    if (platform[platform_moving_arr[i+1]].y >= (platform_moving_arr[i] - platform_move_px))
                        platform[platform_moving_arr[i+1]].y += platform_move_px * platform_moving_dir;
                    else
                        //reset ids so we dont use them
                        platform_moving_arr[i+1] = -1;
                }
            }

            else
            {
                //check to see if all the platform ids have been set
                //then force out the update and continue
                //+=2 because we have 2 elements per
                platforms_left+=2;
                if (platforms_left == platform_moving_arr.length)
                    platform_moving = false;
            }
        }
    }
}

function platform_update()
{
    if ((treat_collects % platform_update_treat_num) == 0)
    {
        if (!platform_update_flag)
        {
            platform_moving_arr = [];
            //count will act as the variable to bitwise AND off
            //times count by 2 e.g. 0000 0001 -> 0000 0010
            let step = lvl_0_tile_inc[(lvl_0_tile_inc_count*2)];
            //find out the move speed via the 2nd index 
            platform_moving_dir = lvl_0_tile_inc[(lvl_0_tile_inc_count*2)+1];
            if (platform_moving_dir == 1)
                platform_moving_dir = platform_move_speed;
            else
                platform_moving_dir = -platform_move_speed;
            for (let i = 0; i < lvl_0_tile_size; i++)
            {
                //if the bit value is 1, move up by 4 ,equal to 0, forget it
                if (step[i] == 1){
                    //i+0 = old platform y (for comparison)
                    platform_moving_arr.push(platform[i].y);
                    //i+1 = the id for platform
                    platform_moving_arr.push(i);
                }
            }

            //if we have some moving platforms, lets tell me
            if (platform_moving_arr.length > 0){
                platform_moving = true;
            }

            //prevent further modifications from happening until the next sequence
            platform_update_flag = true;
            if (lvl_0_tile_inc_count < lvl_0_tile_inc.length)
                lvl_0_tile_inc_count += 1;
        }
    }
}

//the function we invoke in HTML
function _start()
{
    game_area.start();
    //loading in the assets
    main_sprite = new sprite(16, 16, DOG_buffer, DOG_buffer_size, lvl_0_tile_xy[0] - 32, lvl_0_tile_xy[1] - 96, true, 2, true);
    main_sprite.create();
    //count for the lvl x tile xy
    let c = 0;
    for (let i = 0; i < lvl_0_tile_size; i++){
        platform[i] = new sprite(16, 16, PLATFORM_buffer, PLATFORM_buffer_size, lvl_0_tile_xy[c] - 32, lvl_0_tile_xy[c+1] - 32, false, 0, false);
        platform[i].create();
        c+=2;
    }

    //the main game collectables for the player to collect
    treat = new sprite(16, 16, TREAT_buffer, TREAT_buffer_size, 80, 0, true, 1, false);
    treat.create();

    //render water last with transparency
    water_create();

    main_nasty_cat = new nasty_cat();
    //call the nasty cat player lock on method every 20ms
    setInterval(main_nasty_cat_update_func, 20);

    //used for fonts
    game_area.context.font = "12px myFont";

     //icons
    treat_icon = new sprite(16, 16, TREAT_buffer, TREAT_buffer_size, 10, 9, false, 0, false);
    treat_icon.create();
    heart_icon = new sprite(16, 16, HEART_buffer, HEART_buffer_size, 270, 9, false, 0, false);
    heart_icon.create();

    //setting up sounds
    sound_jump = new sound("ASSETS/JUMP.wav");
    sound_jump_land = new sound("ASSETS/JUMP_LAND.wav");
    sound_death = new sound("ASSETS/DEATH.wav");
    sound_get_treat = new sound("ASSETS/GET_TREAT.wav");
    sound_get_treat2 = new sound("ASSETS/GET_TREAT2.wav");
    sound_player_hit = new sound("ASSETS/PLAYER_HIT.wav");
    sound_player_respawn = new sound("ASSETS/RESPAWN.wav");

    //loop through the main game update loop
    requestAnimationFrame(game_area_update);
}

//start by making the game screen area on the HTML page
var game_area =
{
    canvas: document.createElement("canvas"),
    start: function () {
        this.canvas.width = 320;
        this.canvas.height = 200;
        this.context = this.canvas.getContext("2d");
        document.body.insertBefore(this.canvas,
            document.body.childNodes[0]);
    },

    //clear the canvas (like glClear)
    clear: function () {
        this.context.clearRect(0, 0, this.canvas.width, this.canvas.height);
    }
};

function game_area_update()
{
    //move the player sprite
    player_move_zero();

    //more specific collision check for when we are falling off the platform, the collision will be true but 
    //we can still move inside the platform which we don't want.
    //left key
    if (keys.left)
    {
        for (let i = 0; i < lvl_0_tile_size; i++){
            player_left_collision(platform[i]);
        }
        //speed scalar direction for moving the player left
        main_sprite.speedx = -1;
    }

    //right key
    if (keys.right)
    {
        for (let i = 0; i < lvl_0_tile_size; i++){
            player_right_collision(platform[i]);
        }
        //speed scalar direction for moving the player right
        main_sprite.speedx = 1;
    }

    for (let i = 0; i < lvl_0_tile_size; i++){
        player_down_collision(platform[i]);
        player_up_collision(platform[i]);
    }

    //player in water y test 
    if (main_sprite.y <= (game_area.canvas.height - 50))
        main_sprite.in_water = false;
    else if (main_sprite.y > (game_area.canvas.height - 50)){
        sound_death.play();
        main_sprite.in_water = true;
    }

    //respawn the main character once fallen beyond the world
    if (main_sprite.y > (200 + main_sprite.height)){
        if (hearts_left > 0)
            hearts_left-=1;
        main_sprite.x = lvl_0_tile_xy[0] - 32;
        main_sprite.y = lvl_0_tile_xy[1] - 96;
        sound_player_respawn.play();
    }

    //this flag disables if we are falling which also disables double jumping which is good.
    can_jump = true;
    //log the current y
    //we are falling
    if (main_sprite.y > main_sprite_old_y)
        can_jump = false;

    //sync the y back up
    main_sprite_old_y = main_sprite.y;

    //space key to jump, set trigger for jumping
    if (keys.space && !player_jump && can_jump)
    {
        //log the y for the jumping
        main_sprite_jump_y = main_sprite.y;
        player_jump = true;
        main_sprite.gravity_use = false;
        sound_jump.play();
    }

    if (player_jump)
    {
        //if we hit the top boundary without bringing gravity back, we're stuck at pixel 0 
        //because it manipulates y and not speedy
        if (main_sprite.y <= 0){
            main_sprite.gravity_use = true;
            player_jump = false;
        }

        //move the player up till the maximum jump height is achieved, then reintroduce gravity and make us fall.
        if (!main_sprite.in_water)
            main_sprite.speedy = -1;
        else
            main_sprite.speedy = -0.35;

        if (main_sprite.y < (main_sprite_jump_y - main_sprite_max_jump_height)) {
            main_sprite.speedy = 0;
            player_jump = false;
            main_sprite.gravity_use = true;
        }
    }  

    //do collision checks on the player against the treats
    if (main_sprite.collision(treat))
    {
        sound_get_treat.play();
        //if the treat is visible, add it to our conut
        if (treat.visible)
        {
            treat_collects+=1;
            treat.visible = false;
            treat_reset_check(treat);
            //update the platforms if we've collected x number of treats
            platform_update();
            //reset the platform update flag
            platform_update_flag = false;
        }
    }

    platform_move_update();

    //do collision check on player against the main enemy
    if (main_sprite.collision(main_nasty_cat.sprite))
    {
        sound_player_hit.play();
        //remove a life if we get hit
        if (main_nasty_cat.sprite.visible)
        {
            if (hearts_left > 0)
                hearts_left-=1;
            main_nasty_cat.sprite.visible = false;
            main_nasty_cat.reset_pos();
        }
    }

    //clear canvas
    game_area.clear();
    //update render platform
    for (let i = 0; i < lvl_0_tile_size; i++)
        platform[i].render();
    //treat related code, reset_check before update, we alter the speedy
    treat_reset_check(treat);
    treat.render();
    //update and render sprite
    main_sprite.render();
    //main_nasty_cat.update();
    main_nasty_cat.render();
    //render water last with transparency
    water_render();

    //text and icons render
    game_area.context.fillStyle = "#FFCC00";
    game_area.context.fillText("X " + treat_collects, 30, 20); 
    treat_icon.render();
    game_area.context.fillStyle = "#FF0000";
    game_area.context.fillText("X " + hearts_left, 290, 20); 
    heart_icon.render();

    requestAnimationFrame(game_area_update);
}