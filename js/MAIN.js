//black, blue, green, cyan, red, magenta, brown, grey, 
//dark grey, light blue, light green, light cyan, light red,
//light magenta, yellow, white
//this is an array of RGB values for CGA palette above
var vga_palette =
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
var platform;
//bool for tracking player_jump
var player_jump = false;
//for tracking original jump position
var main_sprite_jump_y = 0;
//for testing against new y (used in conjunction with the jumping)
var main_sprite_old_y = 0;
var main_sprite_max_jump_height = 12; 

//array of keys we use for game and windows.keydown/up stores
var keys = 
{
    space: false,
    left: false,
    right: false
};

window.onkeydown = function(e) 
{
    var kc = e.keyCode;
    e.preventDefault();
    if      (kc === 65) keys.left = true;  // only one key per event
    else if (kc === 68) keys.right = true;
    else if (kc === 32) keys.space = true;
};

window.onkeyup = function(e) 
{
    var kc = e.keyCode;
    e.preventDefault();
    if      (kc === 65) keys.left = false;  // only one key per event
    else if (kc === 68) keys.right = false;
    else if (kc === 32) keys.space = false;
};

//the function we invoke in HTML
function _start()
{
    game_area.start();
    main_sprite = new component(16, 16, PLAYER_buffer, PLAYER_buffer_size, 20, 10, true);
    platform = new component(16, 16, PLATFORM_buffer, PLATFORM_buffer_size, 20, 37, false);
}

//start by making the game screen area on the HTML page
var game_area =
{
    canvas: document.createElement("canvas"),
    start : function()
    {
        this.canvas.width = 320;
        this.canvas.height = 200;
        this.context = this.canvas.getContext("2d");
        document.body.insertBefore(this.canvas,
            document.body.childNodes[0]);
        //call game area update every 20ms (50 fps)
        this.interval = setInterval(game_area_update, 20);
    },

    //clear the canvas (like glClear)
    clear: function()
    {
        this.context.clearRect(0, 0, this.canvas.width, this.canvas.height);
    }
}

//adds a component to the game area canvas we are working on
function component(width, height, pix, pix_size, x, y, use_grav)
{
    this.width = width;
    this.height = height;
    this.x = x;
    this.y = y;
    this.speedx = 0;
    this.speedy = 0;
    this.gravity = 0.05
    this.gravity_speed = 0;
    this.gravity_use = use_grav;
   
    this.new_pos = function ()
    {
        //if we collide with the platform, stop the player from falling through
        if (!this.collision(platform)) {
            //if the sprite isn't affect by gravity, dont apply it
            if (this.gravity_use)
                this.speedy = 1;//this.gravity_speed += this.gravity;
        }

        this.x += this.speedx;
        this.y += this.speedy + this.gravity_speed;
    }

    this.collision = function (other)
    {
        var left = this.x;
        var right = this.x + this.width;
        var top = this.y;
        var bottom = this.y + this.height;
        var other_left = other.x;
        var other_right = other.x + other.width;
        var other_top = other.y;
        var other_bottom = other.y + other.height;
        var collide = true;
        if ((bottom < other_top) || (top > other_bottom) || (right < other_left) || (left > other_right))
            collide = false;
        return collide;
    }

    //this is to be called every frame to update component 
    this.update = function ()
    {
        //append the speed velocity x & y onto the position of the component
        //check against general bounding boxes of other components in the scene
        this.new_pos();

        ctx = game_area.context;
        var image_data = ctx.createImageData(this.width, this.height);
        //16 colours, 2 cols per byte (4 bits each)
        //bit shift 4 (* 16) which will get 0-255
        var c = 0;
        for (var i = 0; i < pix_size; i++)
        {
            var first_col = (pix[i] >> 4);
            var second_col = (pix[i] & 0x0F);
            image_data.data[c+0] = vga_palette[(first_col*3)]; //r
            image_data.data[c+1] = vga_palette[(first_col*3)+1]; //g
            image_data.data[c+2] = vga_palette[(first_col*3)+2]; //b
            image_data.data[c+3] = 255; //a
            image_data.data[c+4] = vga_palette[(second_col*3)]; //r
            image_data.data[c+5] = vga_palette[(second_col*3)+1]; //g
            image_data.data[c+6] = vga_palette[(second_col*3)+2]; //b
            image_data.data[c+7] = 255; //a
            c+=8;
        }

        ctx.putImageData(image_data, this.x, this.y);
    }
}

function player_move_zero()
{
    main_sprite.speedx = 0;
    main_sprite.speedy = 0;
}

function game_area_update()
{
    //move the player sprite
    player_move_zero();
    //preferences for the way the player deals with the collisions
    var main_sprite_y_offset = main_sprite.width >> 2; //div by 2
    var platform_width_offset = 2; //extra space on the x for the platform for the player to fall off (game feature)

    //more specific collision check for when we are falling off the platform, the collision will be true but 
    //we can still move inside the platform which we don't want.
    //left key
    if (keys.left)
    {
        //if we come from the right, stop us going into the object
        if (main_sprite.x < ((platform.x + platform.width) + platform_width_offset) &&
            ((main_sprite.y + main_sprite_y_offset) > (platform.y + 1) &&
                (main_sprite.y - main_sprite_y_offset) < ((platform.y + platform.height))))
                    main_sprite.x = (platform.x + platform.width) + platform_width_offset;
        //speed scalar direction for moving the player left
        main_sprite.speedx = -1;
    }

    //right key
    if (keys.right)
    {
        //if we come from the left, stop us going into the object
        if ((main_sprite.x + main_sprite.width) > (platform.x - platform_width_offset) &&
            ((main_sprite.y + main_sprite_y_offset) > (platform.y + 1) &&
                (main_sprite.y - main_sprite_y_offset) < ((platform.y + platform.height))))
            main_sprite.x = (platform.x - main_sprite.width) - platform_width_offset;
        //speed scalar direction for moving the player right
        main_sprite.speedx = 1;
    }


    //log the current y
    var can_jump = true;
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
        main_sprite.speedy = -1;
        if (main_sprite.y < (main_sprite_jump_y - main_sprite_max_jump_height)){
            main_sprite.speedy = 0;
            player_jump = false;
            main_sprite.gravity_use = true;
        }
    }   

    //clear canvas
    game_area.clear();
    //update and render sprite
    main_sprite.update();
    //update render platform
    platform.update();
}