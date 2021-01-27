  function setup() {
      createCanvas(800, 800)
      background(0)
  }
  
  function r(p, w, curve, d, curve0=0, s=1, qq=0) {
      let branchpoint = Math.floor(500*pow(noise(s*(3*TWO_PI), d-s, s+frameCount/304),3))
      for (let i = 0 ; i < 800; i++) {
        qq += 1;
        let t = p5.Vector.sub(p, w)
        let b = p5.Vector.add(p, w)
        line(t.x, t.y, b.x, b.y)
        n = p5.Vector.fromAngle(w.heading() - PI/2)
        w.rotate(curve)
        
        stroke(255, 255-min(qq/2, 255))
        //  curve = curve*0.99 + (random(-1, 1)*0.0004 - (w.heading()- PI/2 - curve0) * 1e-4)*10
  
        curve = curve*0.99 + ((noise(s+i/10*(1+s)/2, 0.4+frameCount/104, s*24*PI)-.5)*0.0004 - (w.heading()- PI/2 - curve0) * 1e-4)*10
  
        //w.mult(1-i/(100000/(3-d)))
        w.setMag(max(0, w.mag()-(7-d)/500))
        p.add(p5.Vector.mult(n, 2))
        if (w.mag() < 0.01 || w.heading() < -PI/2 || w.heading() > 3*PI/2) break;
        if (i > branchpoint && d > 0 && w.mag() > 1) {
          let a = noise(d/3, d+s+frameCount/55*s*d, s*2*TWO_PI)
          let p1 = p5.Vector.add(p, p5.Vector.mult(w, 1-a))
          let p2 = p5.Vector.add(p, p5.Vector.mult(w, -a))
          let w1 = p5.Vector.mult(w, a)
          let w2 = p5.Vector.mult(w, (1-a))
          r(p1, w1, curve, d-1, curve0 + 0.2+noise(0.3+frameCount/305, s, d)/5, s - pow(0.5, 3-d), qq)
          r(p2, w2, curve, d-1, curve0 - 0.2-noise(frameCount/285, d, s)/5, s + pow(0.5, 3-d), qq)
          break
        }
      }
  }
  
  function draw() {
    strokeWeight(8)
    if (frameCount % 10 > 5)
      background(0, 30/3)
    rotate(0, PI)
    let p = createVector(0, height/2)
    let w = createVector(0, 50)
    let curve = 0
    r(p, w, curve, 3)
  }