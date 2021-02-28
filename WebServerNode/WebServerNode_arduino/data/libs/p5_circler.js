class p5_circler{
    dom = {};
    active = false;
    channels = [NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN];
  
    constructor(element){
        this.dom = document.createElement('div');
        this.dom.id = "circler1"
        this.dom.style.display = "none";
        element.appendChild(this.dom);
        this.circler = new p5(circler,this.dom.id);
    }
  
    set_Channels = function(set_channels){
        for(var i = 0; i <set_channels.length; i++){
            if(i > this.channels.length){break;}
            if(!isNaN(set_channels[i])){
              this.channels[i] = +set_channels[i];
            }
        }
        this.circler.settings(this.channels);
    }
  
    ChangeSpeed = function(){
        console.log("todo");
    }
    resize = function(){
        // fixed in p5
    }
  
    setAnimotor = function(ClaSS){
        this.dom.style.display = 'block';
        this.circler.play();
        this.active = true;
    }
    clearAnimotor = function(){
        this.dom.style.display = 'none';
        this.active = false;
        this.circler.pause();
    }
  }


var circler = function( p5func ) {
    let rgb = [50,50,50]

    p5func.setup = function() {
        p5func.createCanvas(width,height);
    };
  
    p5func.play = function(){
        p5func.loop()
    }
  
    p5func.pause = function(){
        p5func.noLoop()
    }

    p5func.settings = function(channels){
        rgb[0]   = channels[0]/2;
        rgb[1]   = channels[1]/2;
        rgb[2]   = channels[2]/2;
    };

    p5func.draw = function() {
        // p5func.background(25+rgb[0],25+rgb[1],25+rgb[2],10);
        p5func.background(25,5);
        p5func.strokeWeight(4);
        p5func.stroke(25+rgb[0],25+rgb[1],25+rgb[2])
        let a = p5func.createVector(width/2, height/2)
        let b = p5func.createVector(1, 0)
        b.rotate(0.5*p5func.noise((p5func.frameCount+-1*2346)/100))
        b.rotate(-p5func.frameCount/100)
        for (let i = 0; i < 300; i++) {
            let c = p5.Vector.add(a, b)
            // p5func.stroke(10+60*p5func.sin(p5func.frameCount/5+i/50)) // extra sin door lijn 
            p5func.line(a.x, a.y, c.x, c.y);
            a = c
            b.setMag(b.mag() * 1.015)
            b.rotate(0.35*p5func.noise((p5func.frameCount+i*2346)/140))
        }
    };

    p5func.windowResized = function() {
        p5func.resizeCanvas(p5func.windowWidth, p5func.windowHeight);

    }
}