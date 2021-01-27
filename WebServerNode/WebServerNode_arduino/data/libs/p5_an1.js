//Channels 1 2 3 --> inUSE ATM rgb
class p5_an1{
    dom = {};
    active = false;
    channels = [NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN];
  
    constructor(element){
        this.dom = document.createElement('div');
        this.dom.id = "an1"
        this.dom.style.display = "none";
        element.appendChild(this.dom);
        this.an1 = new p5(an1,this.dom.id);
    }
  
    set_Channels = function(set_channels){
        for(var i = 0; i <set_channels.length; i++){
            if(i > this.channels.length){break;}
            if(!isNaN(set_channels[i])){
              this.channels[i] = +set_channels[i];
            }
        }
        this.an1.settings(this.channels);
    }
  
    resize = function(){
        // console.log("todo");
    }
  
    setAnimotor = function(ClaSS,speed){
        this.dom.style.display = 'block';
        this.an1.play(speed);
        this.active = true;
    }
    clearAnimotor = function(speed){
        this.dom.style.display = 'none';
        this.active = false;
        this.an1.pause();
    }
  }

var an1 = function( p5func ) {
    let cells = []
    let red   = 1
    let green = 1
    let blue  = 255
    const r0  = 3
    const N   = 200

    
    p5func.play = function(set_speed){
        p5func.loop()
    }
  
    p5func.pause = function(){
        p5func.noLoop()
    }
    
    p5func.settings = function(channels){
        red   = channels[0];
        green = channels[1];
        blue  = channels[2];
        cells.forEach(c => {
            c.blue = blue
            c.red  = red
            c.green = green
        })  
    };


    p5func.reset = function(){
        cells = []
        cells.push(new P(red,green,blue))
        cells.push(new P(red,green,blue))
        cells.push(new P(red,green,blue))
    };



    class P {
        constructor(redi,greeni,bluei) {
            this.green = bluei
            this.blue = greeni
            this.red = redi
            let dividing = 0
            let ps = this.ps = []
            for (let i = 0; i < N; i++) {
                let p = p5func.createVector(p5func.width / 2 + 50 * p5func.cos(i / N * p5func.TWO_PI), p5func.height / 2 + 50 * p5func.sin(i / N * p5func.TWO_PI))
                ps.push(p)
            }
            this.id = cells.length
        }

        draw() {
            p5func.fill(this.red, this.green, this.blue, (this.ps.length / 20))
            let ps = this.ps
            let a = 0
            let b = Math.floor(ps.length / 2)
            let CM = p5func.createVector(0, 0)
            
            if (this.ps.length < N) {
                this.ps.push(this.ps[0].copy().add(this.ps[this.ps.length - 1]).mult(0.5))
            }

            if (this.dividing > 50) {
                this.dividing = 0
                let ch = new P(this.red,this.green,this.blue);
                ch.ps = []
                for (let i = ps.length - 1; i > b; i--) {
                    ch.ps.push(this.ps[i])
                }
                this.ps.length = b - 1
                cells.push(ch)
                this.dividing = 0
            }
            ps.forEach(p => {
                CM.add(p)
            })
            CM.div(ps.length)
            
            let CM0 = ps.length * r0 / p5func.PI

            let dps = ps.map((p, i) => {
                let l = ps[(i + ps.length - 1) % ps.length].copy().sub(p)
                let r = ps[(i + 1) % ps.length].copy().sub(p)
                let c = CM.copy().sub(p)
                let ll = l.mag()
                let lr = r.mag()
                let lc = c.mag()
                l.normalize()
                r.normalize()
                c.normalize()
                l.mult(ll - r0).mult(4)
                r.mult(lr - r0).mult(4)
                c.mult(0.2 * (lc - CM0))
                let F = p5.Vector.add(l, r).add(c).mult(0.1)
                F.x += (p5func.noise(this.id * p5func.PI + .01 * p.x, this.id * 100 + 0.05 * p.y, p5func.frameCount / (40 + this.id)) - .5) * 2
                F.y += (p5func.noise(this.id * p5func.PI + .01 * p.y, this.id * 100 + 0.05 * p.x, p5func.frameCount / (40 + this.id)) - .5) * 2
                return F
            })

            if (this.dividing > 0) {
                let d = ps[b].copy().sub(ps[a])
                let l = d.mag()
                d.normalize().mult(l).mult(this.dividing / 120)
                dps[a].add(d)
                dps[b].sub(d)
                this.dividing += 1
            }

            dps.forEach((dp, i) => {
                ps[i].add(dp)
            })
            
            p5func.beginShape()
            p5func.vertex(ps[0].x, ps[0].y)
            for (let i = 0; i < ps.length; i++) {
                p5func.quadraticVertex(ps[i].x, ps[i].y, ps[(i + 1) % ps.length].x, ps[(i + 1) % ps.length].y)
            }
            p5func.endShape()
        }
    }

    p5func.setup = function() {
        p5func.createCanvas(width,height);
        cells.push(new P(red,green,blue))
        p5func.background(0)
    };

    p5func.draw = function() {
        p5func.background(0,0,0,10)
        p5func.stroke(red, green, blue, 168)
        //noStroke()
        
        cells.forEach(p => p.draw())
        if (p5func.frameCount % 400 == 0 && cells.length < 64) {
            cells.forEach(c => {
            if (p5func.random() < 0.9 && c.ps.length == N)
                c.dividing = 1
            })
        }
        if (p5func.frameCount % 50 == 0 && cells.length >= 2 && p5func.random() < 0.2) {
            let idx = Math.floor(p5func.random(cells.length))
            cells.splice(idx, 1)
        }
        if(cells.length >= 32){

        }
    };

    p5func.windowResized = function() {
        p5func.resizeCanvas(p5func.windowWidth, p5func.windowHeight);
    }
};