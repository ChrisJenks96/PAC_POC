//the function we invoke in HTML

var main_sprite;

function _start()
{
    game_area.start();
    main_sprite = new component(16, 16, "red", 10, 10);
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
function component(width, height, color, x, y)
{
    this.width = width;
    this.height = height;
    this.x = x;
    this.y = y;
    //this is to be called every frame to update component 
    this.update = function (){
        ctx = game_area.context;
        ctx.fillStyle = color;
        ctx.fillRect(this.x, this.y, this.width, this.height);
    }
}

function game_area_update()
{
    //clear canvas
    game_area.clear();
    //render sprite
    main_sprite.update();
}