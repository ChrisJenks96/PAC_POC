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
//preferences for the way the player deals with the collisions
const main_sprite_y_offset = 8//main_sprite.width >> 2; //div by 2
const platform_width_offset = 2; //extra space on the x for the platform for the player to fall off (game feature)
//array of keys we use for game and windows.keydown/up stores

const num_platforms = 10;

//the water pixel data
var water_data = [];

var keys = 
{
    space: false,
    left: false,
    right: false
};

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

//the function we invoke in HTML
function _start()
{
    game_area.start();
    //loading in the assets
    main_sprite = new sprite(16, 16, DOG_buffer, DOG_buffer_size, 20, 10, true, 2, true);
    main_sprite.create();
    for (let i = 0; i < num_platforms; i++){
        //between 20 and 180
        let rand_y_pos = Math.random() * 100;
        platform[i] = new sprite(16, 16, PLATFORM_buffer, PLATFORM_buffer_size, 20 + (i * 25), 40 + rand_y_pos, false, 0, false);
        platform[i].create();
    }

    treat = new sprite(16, 16, TREAT_buffer, TREAT_buffer_size, 160, 0, true, 1, false);
    treat.create();

    //render water last with transparency
    for (let i = 0; i < game_area.canvas.width; i++)
        water_create(i);
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

//adds a component to the game area canvas we are working on
function sprite(width, height, pix, pix_size, x, y, use_grav, grav_value, bound_check)
{
    this.width = width;
    this.height = height;
    this.x = x;
    this.y = y;
    this.speedx = 0;
    this.speedy = 0;
    this.gravity_use = use_grav;
    this.gravity_value = grav_value;
    this.image_data = game_area.context.createImageData(this.width, this.height);
    //if we want to do the screen bound check, enable this
    this.bound_check = bound_check;
    this.in_water = false;
   
    //creates the sprite and parses the pixels from the 
    //palette indexed by the included sprite JS files
    //created with BIN_TO_JS
    this.create = function()
    {
        ctx = game_area.context;
        //16 colours, 2 cols per byte (4 bits each)
        //bit shift 4 (* 16) which will get 0-255
        let c = 0;
        for (let i = 0; i < pix_size; i++) {
            //XXXX YYYY = 0000 XXXX
            let first_col = pix[i] >> 4;
            //XXXX YYYY & 0000 FFFF = 0000 YYYY
            let second_col = pix[i] & 0x0F;
            //parse under base 16 hex
            this.image_data.data[c + 0] = parseInt(cga_palette[(first_col * 3)], 16); //r
            this.image_data.data[c + 1] = parseInt(cga_palette[(first_col * 3) + 1], 16); //g
            this.image_data.data[c + 2] = parseInt(cga_palette[(first_col * 3) + 2], 16); //b
            this.image_data.data[c + 3] = 255; //a
            this.image_data.data[c + 4] = parseInt(cga_palette[(second_col * 3)], 16); //r
            this.image_data.data[c + 5] = parseInt(cga_palette[(second_col * 3) + 1], 16); //g
            this.image_data.data[c + 6] = parseInt(cga_palette[(second_col * 3) + 2], 16); //b
            this.image_data.data[c + 7] = 255; //a
            c += 8;
        }
    }

    this.new_pos = function ()
    {
        //if the sprite isn't affect by gravity, dont apply it
        if (this.gravity_use) {
            //fall at normal gravity rate if not in water
            if (!this.in_water)
                this.speedy = this.gravity_value;//this.gravity_speed += this.gravity;
            else //fall at reduced speed during water
                this.speedy = 0.35;
        }

        this.x += this.speedx;
        this.y += this.speedy;

        //bounds check
        if (this.bound_check)
        {
            if (this.x > (320 - this.width))
                this.x = 320 - this.width;
            if (this.x < 0)
                this.x = 0;
            if (this.y > (200 - this.height))
                this.y = 200 - this.height;
            if (this.y < 0)
                this.y = 0;
        }
    }

    //this is to be called every frame to update component 
    this.update = function ()
    {
        //append the speed velocity x & y onto the position of the component
        //check against general bounding boxes of other components in the scene
        this.new_pos();
        ctx = game_area.context;
        ctx.putImageData(this.image_data, this.x, this.y);
    }
}

//a bounds check for the treats, they fall out of world, we bring them back (obj pooling)
function treat_reset_check(treat)
{
    if (treat.y >= game_area.canvas.height)
    {
        treat.speedy = -0.5; //random speed vector for falling
        treat.y = -30; //reset off screen
        treat.x = Math.random() * game_area.canvas.height;
    }
}

function water_create(index)
{
    let c = 0;
    let ctx = game_area.context;
    //do the x one frame at a time 
    water_data[index] = ctx.createImageData(1, 50);
    //flip the y to bring the waves to the top 
    for (let y = 50; y > 0; y--) {
        //the sine wave
        let y_sine_offset = (4 * Math.sin(0.125 * index));
        //dont render out any pixels if we are higher than the peak
        if (y > y_sine_offset) {
            //parse under base 16 hex
            water_data[index].data[c] = parseInt(cga_palette[(9 * 3)], 16); //r
            water_data[index].data[c + 1] = parseInt(cga_palette[(9 * 3) + 1], 16); //g
            water_data[index].data[c + 2] = parseInt(cga_palette[(9 * 3) + 2], 16); //b
            water_data[index].data[c + 3] = 255; //a
        }

        c += 4;
    }
}

//This renders out the water data sprite we generated in water_create
function water_update(index)
{
    let ctx = game_area.context;
    for (let y = 50; y > 0; y--) {
        let y_sine_offset = (4 * Math.sin(0.125 * index));
        ctx.putImageData(water_data[index], index, y_sine_offset + (game_area.canvas.height - 32));
    }
}

//reset the velocity vectors
function player_move_zero()
{
    main_sprite.speedx = 0;
    main_sprite.speedy = 0;
}

function player_left_collision(other)
{
    //if we come from the left, stop us going into the object
    //other.y + 1 just add more of a bigger boundary for the player to hit
     if ((main_sprite.x - other.width) > other.x && main_sprite.x < ((other.x + other.width) + platform_width_offset) &&
        ((main_sprite.y + main_sprite_y_offset) > (other.y + 1) &&
            (main_sprite.y - main_sprite_y_offset) < ((other.y + other.height))))
                main_sprite.x += 1;//(other.x + other.width) + platform_width_offset;
}

function player_right_collision(other)
{
    //if we come from the right, stop us going into the object
    if ((main_sprite.x + main_sprite.width) < other.x && (main_sprite.x + main_sprite.width) > (other.x - platform_width_offset) &&
        ((main_sprite.y + main_sprite_y_offset) > (other.y) &&
            (main_sprite.y - main_sprite_y_offset) < ((other.y + other.height))))
                main_sprite.x -= 1;//(other.x - main_sprite.width) - platform_width_offset;
}

function player_down_collision(other)
{
    if ((main_sprite.y + main_sprite.height) >= other.y && 
        (main_sprite.x + main_sprite.width) > other.x && 
            main_sprite.x < (other.x + other.width))
                main_sprite.y -= 2;
}

function player_up_collision(other)
{
    if (main_sprite.y >= (other.y + other.height) && 
        (main_sprite.x + main_sprite.width) > other.x && 
            main_sprite.x < (other.x + other.width))
                main_sprite.y += 2;
}

function game_area_update()
{
    //move the player sprite
    player_move_zero();

    //more specific collision check for when we are falling off the platform, the collision will be true but 
    //we can still move inside the platform which we don't want.
    //left key
    if (keys.left)
    {
        for (let i = 0; i < num_platforms; i++){
            player_left_collision(platform[i]);
        }
        //speed scalar direction for moving the player left
        main_sprite.speedx = -1;
    }

    //right key
    if (keys.right)
    {
        for (let i = 0; i < num_platforms; i++){
            player_right_collision(platform[i]);
        }
        //speed scalar direction for moving the player right
        main_sprite.speedx = 1;
    }

    for (let i = 0; i < num_platforms; i++){
        player_down_collision(platform[i]);
        player_up_collision(platform[i]);
    }

    //player in water y test 
    if (main_sprite.y <= (game_area.canvas.height - 50))
        main_sprite.in_water = false;
    else if (main_sprite.y > (game_area.canvas.height - 50))
        main_sprite.in_water = true;

    //this flag disables if we are falling which also disables double jumping which is good.
    let can_jump = true;
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

    //clear canvas
    game_area.clear();
    //update render platform
    for (let i = 0; i < num_platforms; i++)
        platform[i].update();
    //treat related code, reset_check before update, we alter the speedy
    treat_reset_check(treat);
    treat.update();
    //update and render sprite
    main_sprite.update();
    //render water last with transparency
    for (let i = 0; i < game_area.canvas.width; i++)
        water_update(i);
    requestAnimationFrame(game_area_update);
}