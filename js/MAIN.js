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
        window.addEventListener('keydown', function (e) {
            game_area.key = e.keyCode;
        })
        window.addEventListener('keyup', function (e) {
            game_area.key = false;
        })
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

        //if we come from the right, stop us going into the object
        if (this.x < ((platform.x + platform.width) + 2) &&
               (this.y > (platform.y + 1) && this.y < ((platform.y + platform.height))))
            this.x = (platform.x + platform.width) + 2;
        //if we come from the left, stop us going into the object
        else if (((this.x - this.width) > platform.x - 2) &&
            (this.y > (platform.y + 1) && this.y < ((platform.y + platform.height))))
            this.x = (platform.x - platform.width) - 2;

        this.x += this.speedx;
        this.y += this.speedy + this.gravity_speed;
        //if (this.gravity_use)
            //this.hit_floor();
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

    this.hit_floor = function ()
    {
        //37 height of base platform
        //+1 to offset the black boundary around the sprite (ASM refresh hack, not JS related)
        var floor_y = platform.y - (platform.height - 1);
        if (this.y > floor_y)
            this.y = floor_y;
    }

    //this is to be called every frame to update component 
    this.update = function () {

        this.new_pos();

        ctx = game_area.context;
        ctx.imageSmoothingEnabled = false;
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
    if (game_area.key == 37)
        main_sprite.speedx = -1;
    if (game_area.key == 39)
        main_sprite.speedx = 1;

    //clear canvas
    game_area.clear();
    //update and render sprite
    main_sprite.update();
    //update render platform
    platform.update();
}