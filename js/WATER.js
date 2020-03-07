//the water pixel data
var water_data = [];

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
            water_data[index].data[c] = parseInt(cga_palette[(1 * 3)], 16); //r
            water_data[index].data[c + 1] = parseInt(cga_palette[(1 * 3) + 1], 16); //g
            water_data[index].data[c + 2] = parseInt(cga_palette[(1 * 3) + 2], 16); //b
            water_data[index].data[c + 3] = 255; //a
        }

        c += 4;
    }
}

//This renders out the water data sprite we generated in water_create
function water_render(index)
{
    let ctx = game_area.context;
    for (let y = 50; y > 0; y--) {
        let y_sine_offset = (4 * Math.sin(0.125 * index));
        ctx.putImageData(water_data[index], index, y_sine_offset + (game_area.canvas.height - 32));
    }
}