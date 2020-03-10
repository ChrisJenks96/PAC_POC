//preferences for the way the player deals with the collisions
const main_sprite_y_offset = 8//main_sprite.width >> 2; //div by 2
const platform_width_offset = 2; //extra space on the x for the platform for the player to fall off (game feature)

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
    this.visible = true;

    this.collision = function(other)
    {
        var myleft = this.x;
        var myright = this.x + (this.width);
        var mytop = this.y;
        var mybottom = this.y + (this.height);
        var otherleft = other.x;
        var otherright = other.x + (other.width);
        var othertop = other.y;
        var otherbottom = other.y + (other.height);
        var collision = true;
        if ((mybottom < othertop) || (mytop > otherbottom) ||
            (myright < otherleft) || (myleft > otherright)) {
                collision = false;
        }

        return collision;
    }
   
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
            if (this.y < 0)
                this.y = 0;
        }
    }

    //this is to be called every frame to update component 
    this.render = function ()
    {
        //append the speed velocity x & y onto the position of the component
        //check against general bounding boxes of other components in the scene
        this.new_pos();
        if (this.visible)
        {
            ctx = game_area.context;
            ctx.putImageData(this.image_data, this.x, this.y);
        }
    }
}

//static function so we can call setInterval
function main_nasty_cat_update_func()
{
    main_nasty_cat.update();
}

//store all the nasty cat functionality in this structure
function nasty_cat()
{
    //the sample directional vectors
    this.old_dir_x = [];
    this.old_dir_y = [];
    //final directional vector the cat will follow
    this.dir_x;
    this.dir_y;
    this.cycle = 0;
    this.speed = 0.125;
    //the main baddies in the game, the nasty cats are out to ruin you
    this.sprite = new sprite(16, 16, NASTY_CAT_buffer, NASTY_CAT_buffer_size, 150, -10, false, 1, false);
    this.sprite.create();

    this.render = function()
    {
        this.sprite.render();
    }

    this.update = function()
    {
        //get 2 samples of the cat following the player
        //from them two position, subtract to get directional vector
        //then force the directional vector on the player, static but 
        //means the nasty cat will have a general idea of where the player is
        if (this.cycle < 2)
        {
            this.old_dir_x[this.cycle] = (main_sprite.x - this.sprite.x);
            this.old_dir_y[this.cycle] = (main_sprite.y - this.sprite.y);
            this.sprite.x += this.old_dir_x[this.cycle] * -this.speed;
            this.sprite.y += this.old_dir_y[this.cycle] * -this.speed;
            this.cycle++;
        }

        else if (this.cycle == 2)
        {
            //get the sample data and make our directional vector
            this.dir_x = this.old_dir_x[1] - this.old_dir_x[0];
            this.dir_y = this.old_dir_y[1] - this.old_dir_y[0];
            //force it out of these two config statements
            this.cycle = 3;
        }

        else if (this.cycle > 2)
        {
            this.sprite.x += this.dir_x * this.speed;
            this.sprite.y += this.dir_y * this.speed;
        }

        this.bounds_check();
    }

    this.reset_pos = function()
    {
        //reset position back, random x, we need the cat out of shot
        this.sprite.x = 150;
        this.sprite.y = -50;
        //cycle 0 resets the sample data for direction
        this.cycle = 0;
        this.sprite.visible = true;
    }

    this.bounds_check = function()
    {
         //bounds checks, if the cat is out, reset it and do it again
        let out_of_bounds = false;
        if (this.sprite.x > (320 + this.sprite.width))
            out_of_bounds = true;
        if (this.sprite.x < 0 - this.sprite.width)
            out_of_bounds = true;
        if (this.sprite.y > (200 + this.sprite.height))
            out_of_bounds = true;

        if (out_of_bounds)
            this.reset_pos();
    }
};

//a bounds check for the treats, they fall out of world, we bring them back (obj pooling)
function treat_reset_check(treat)
{
    if (treat.y >= game_area.canvas.height)
    {
        treat.speedy = -0.5; //random speed vector for falling
        treat.y = -30; //reset off screen
        treat.x = Math.random() * game_area.canvas.height;
        treat.visible = true;
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
            main_sprite.x < (other.x + other.width)){
                if (!can_jump)
                    sound_jump_land.play();
                main_sprite.y -= 2;
            }
}

function player_up_collision(other)
{
    if (main_sprite.y >= (other.y + other.height) && 
        (main_sprite.x + main_sprite.width) > other.x && 
            main_sprite.x < (other.x + other.width))
                main_sprite.y += 2;
}