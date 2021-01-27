//Channels 1 2 3 --> inUSE ATM rgb
class p5_Phantom_town{
    dom = {};
    active = false;
    channels = [NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN];

    constructor(element){
        this.dom = document.createElement('div');
        this.dom.id = "Phantom_town"
        this.dom.style.display = "none";
        element.appendChild(this.dom);
        this.p5_hol_camp = new p5(Phantom_town,this.dom.id);
    }

    set_Channels = function(set_channels){
        for(var i = 0; i <set_channels.length; i++){
            if(i > this.channels.length){break;}
            if(!isNaN(set_channels[i])){
                this.channels[i] = +set_channels[i];
            }
        }
        this.p5_hol_camp.settings(this.channels);
    }

    resize = function(){
        console.log("todo");
    }

    setAnimotor = function(ClaSS,speed){
        this.dom.style.display = 'block';
        this.p5_hol_camp.play(speed);
        this.active = true;
    }
    clearAnimotor = function(speed){
        this.dom.style.display = 'none';
        this.active = false;
        this.p5_hol_camp.pause();
    }
}

//P5 functionality
var Phantom_town = function(p5func) {
    let red = 255
    let green = 0
    let blue = 0
    
    let speed = 0.00002
    let a = 0.9
    let b = 2

    width     =  p5func.windowWidth
    height    =  p5func.windowHeight  


    p5func.settings = function(channels){
        red   = channels[0]/2;
        green = channels[1]/2;
        blue  = channels[2]/2;
        // white = channels[3];

        // console.log(channels);
        // a     = channels[4];
        // b     = channels[5];
        // c     = channels[6];
    };

    p5func.play = function(set_speed){
        speed = set_speed
        p5func.loop()
    }

    p5func.pause = function(){
        p5func.noLoop()
    }

    p5func.setup = function() {
        p5func.createCanvas(width, height)
        p5func.noLoop()
    };

    p5func.spiral = function(x, y, a=1, res=10,div=0.001) {
        p5func.push()
        p5func.translate(x, y)
        let begin = p5func.createVector(0, 0)
        let to = p5func.createVector(0, res)

        for (let i = 0; i < 100; i++) {
            let end = p5.Vector.add(begin, to)
            to.rotate(p5func.pow(0.4*10/res + p5func.sin(p5func.frameCount*div), 1/a)).mult(p5func.pow(1.1, a))
            p5func.line(begin.x, begin.y, end.x, end.y) 
            begin = end
        }
        p5func.pop()
    };

    p5func.draw = function() {
        // p5func.background(100+red,100+green,100+blue, 40)
        p5func.background(25,5);
        p5func.background(50, 40)
        p5func.stroke(25+red,25+green,25+blue)
        p5func.spiral(width/2, height/2,a,b,speed)
    };
    
    p5func.windowResized = function() {
        p5func.resizeCanvas(p5func.windowWidth, p5func.windowHeight);
    }

}