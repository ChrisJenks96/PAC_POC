function water_create()
{
    //this will be used for a water effect later down the line
}

//This renders out the water data sprite we generated in water_create
function water_render()
{
    ctx.fillStyle = '#0000A8';
    game_area.context.fillRect(0, 200 - 32, 320, 200);
}