//Channels 1 2 3 --> inUSE ATM rgb
class p5_hol_camp{
  dom = {};
  active = false;
  channels = [NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN];

  constructor(element){
      this.dom = document.createElement('div');
      this.dom.id = "hol_camp"
      this.dom.style.display = "none";
      element.appendChild(this.dom);
      this.p5_hol_camp = new p5(hol_camp,this.dom.id);
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
      // console.log("todo");
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



//P5 functionality!
var hol_camp = function( p5func ) {
    width     = p5func.windowWidth
    height    =  p5func.windowHeight
  
    let colorb     = [0,0,0,20]
    let colorc1    = [0,255,255]
    let colorc2    = [255,0,255]
    let colorc3    = [255,255,0]

    let points_1 = []
    let points_2 = []
    let points_3 = []

    p5func.play = function(set_speed){
      p5func.loop()
    }

    p5func.pause = function(){
        p5func.noLoop()
    }

    p5func.reset2 = function(){
      console.log("reset2")
      points_2 = []
    };

    p5func.settings = function(channels){
      if(channels[3] == 0){
        colorc1 = [channels[0],channels[1],channels[2]]
        colorc2 = [channels[0],channels[1],channels[2]]
        colorc3 = [channels[0],channels[1],channels[2]]
      }else{
        // let red   = (channels[3] + this.channels[0]/2.56)%256
        // let green = (channels[3] + this.channels[1]/2.56)%256
        // let blue  = (channels[3] + this.channels[2]/2.56)%256
        
        // colorc1 = [red,green,blue]
        // colorc2 = [red,green,blue]
        // colorc3 = [red,green,blue]
      }

    };

    p5func.setup = function() {
      p5func.createCanvas(width, height)
      for (let i = 0; i < 50; i++) {
        points_1.push({
          x: p5func.random(p5func.width/4-50,p5func.width/4+50),
          y: p5func.random(0, 100),
          vx: 0,
          vy: 0
        })
      }
      for (let i = 0; i < 50; i++) {
        points_2.push({
          x: p5func.random(p5func.width/4*2-50,p5func.width/4*2+50),
          y: p5func.random(0, 100),
          vx: 0,
          vy: 0
        })
      }
      for (let i = 0; i < 50; i++) {
        points_3.push({
          x: p5func.random(p5func.width/4*3-50,p5func.width/4*3+50),
          y: p5func.random(0, 100),
          vx: 0,
          vy: 0
        })
      }
    }

    function ng(i, j) {
      let d = 10
      let dx = (p5func.noise((i+d)/50, j/50, p5func.frameCount/10) - p5func.noise((i-d)/50, j/50, p5func.frameCount/10)) / d
      let dy = (p5func.noise(i/50, (j+d)/50, p5func.frameCount/10) - p5func.noise(i/50, (j-d)/50, p5func.frameCount/10)) / d
      return [dx, dy]
    }

    const R = 50
    p5func.draw = function(){
      
      p5func.background(colorb)
      p5func.noStroke()

      p5func.fill(colorc1)
      points_1.forEach(p => {
        let [grad_x, grad_y] = ng(p.x, p.y)
        z = p5func.noise(p.x/10, p.y/10)
        p.vx = 0.97*p.vx + grad_x * (1-z) * 20
        p.vy = 3.5*z + grad_y*10
      })
      points_1.forEach(p => {
        if (p5func.abs(p.vx) > 2) p.vx = p5func.abs(p.vx) / p.vx * 2
        p.x += p.vx
        p.y += p.vy
        p5func.circle(p.x, p.y, 10)
        if (p5func.random() > 0.995) {
          p.x = p5func.random(p5func.width/4-50, p5func.width/4+50),
          p.y = p5func.random(0,0)
        }
      })

      p5func.fill(colorc2)
      points_2.forEach(p => {
        let [grad_x, grad_y] = ng(p.x, p.y)
        z = p5func.noise(p.x/10, p.y/10)
        p.vx = 0.97*p.vx + grad_x * (1-z) * 20
        p.vy = 3.5*z + grad_y*10
      })
      points_2.forEach(p => {
        if (p5func.abs(p.vx) > 2) p.vx = p5func.abs(p.vx) / p.vx * 2
        p.x += p.vx
        p.y += p.vy
        p5func.circle(p.x, p.y, 10)
        if (p5func.random() > 0.995) {
          p.x = p5func.random(p5func.width/4*2 -50, p5func.width/4 *2+50),
          p.y = p5func.random(0,0)
        }
      })
      p5func.fill(colorc3)
      points_3.forEach(p => {
        let [grad_x, grad_y] = ng(p.x, p.y)
        z = p5func.noise(p.x/10, p.y/10)
        p.vx = 0.97*p.vx + grad_x * (1-z) * 20
        p.vy = 3.5*z + grad_y*10
      })
      points_3.forEach(p => {
        if (p5func.abs(p.vx) > 2) p.vx = p5func.abs(p.vx) / p.vx * 2
        p.x += p.vx
        p.y += p.vy
        p5func.circle(p.x, p.y, 10)
        if (p5func.random() > 0.995) {
          p.x = p5func.random(p5func.width/4*3 -50, p5func.width/4*3+50),
          p.y = p5func.random(0,0)
        }
      })

    }

    p5func.windowResized = function() {
      p5func.resizeCanvas(p5func.windowWidth, p5func.windowHeight);
  }
};