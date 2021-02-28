//Channels 1 2 3 5 7--> inUSE ATM rgbw
class Block_Animation{
    dom = {}
    //Channels 0 1 2 3 5 --> inUSE ATM  4 reserved for strobo
    channels = [255,0,0,0,0,0,0,75]; 
    size  = 250;

    Animation = function(){
        for (var i = this.dom.Blocks.length; i > 0 ; i--) {
            var block = this.dom.Blocks[i-1];
            var saturation = Math.random();
            var r = (saturation * this.channels[0]);
            var g = (saturation * this.channels[1]);
            var b = (saturation * this.channels[2]);
            var w = (saturation * this.channels[3]);
            if(w){
                r = (w + this.channels[0]/2.56)%256;
                g = (w + this.channels[1]/2.56)%256;
                b = (w + this.channels[2]/2.56)%256;
            }            
            block.style.backgroundColor = 'rgb('+r+','+g+','+b+')';
        }
    }

    constructor(element){
        this.dom = document.createElement('div');
        this.dom.id = 'Block_Animation';
        element.appendChild(this.dom);
        this.dom.style.width  =  '100%';
        this.dom.style.height =  '100%';
        this.dom.style.position = 'absolute';
        this.dom.style.marginLeft   = 'auto';
        this.dom.style.marginRight  = 'auto';
        this.dom.style.marginTop    = 'auto';
        this.dom.style.marginBottom = 'auto';
        this.dom.style.top = 0; 
        this.dom.style.bottom = 0;
        this.dom.style.left = 0;
        this.dom.style.right = 0;
        this.dom.style.display = 'none';
        this.dom.Blocks = [];
        this.resize()
    }
    resize        = function(){
        //remove the blocks
        for (var i = this.dom.Blocks.length; i > 0 ; i--) {
            this.dom.Blocks[i-1].remove();
            this.dom.Blocks.splice(i-1,1);    
        }
        //Create the blocks
        if(this.size){
            var w =  document.getElementById('mainsrc').clientWidth;
            var h =  document.getElementById('mainsrc').clientHeight;
            var nbW = Math.floor(w/this.size);
            var nbH = Math.floor(h/this.size);

            this.dom.style.width  =  this.size*nbW + 'px';
            this.dom.style.height =  this.size*nbH + 'px';

            if(!(nbW*nbH)){
                this.dom.style.width  =  '100%';
                this.dom.style.height =  '100%';
                var block = {}
                block = document.createElement('div');
                block.className = 'Animation_block';                
                block.style.width =  '100%'; 
                block.style.height = '100%'; 
                this.dom.appendChild(block);
                this.dom.Blocks.push(block);
            }
            for(var i=0;i<(nbW*nbH);i++){
                var block = {}
                block = document.createElement('div');
                block.className    = 'Animation_block';                
                block.style.width  =  String(this.size) + 'px'; 
                block.style.height =  String(this.size) + 'px'; 
                block.style.float  =  'left';
                block.style.webkitTransition = 'background-color 0.2s ease-in';
                block.style.MozTransition    = 'background-color 0.2s ease-in';
                block.style.msTransition     = 'background-color 0.2s ease-in';
                block.style.OTransition      = 'background-color 0.2s ease-in';
                this.dom.appendChild(block);
                this.dom.Blocks.push(block);
            }
        }
        else {
            //todo
            this.dom.style.width  =  '100%';
            this.dom.style.height =  '100%';
            var block = {}
            block = document.createElement('div');
            block.className = 'Animation_block';                
            block.style.width =  '100%'; 
            block.style.height = '100%'; 
            this.dom.appendChild(block);
            this.dom.Blocks.push(block);
        }
    }
    set_Channels  = function(set_channels){
        for(var i = 0; i <set_channels.length; i++){
            if(i > this.channels.length){break;}
            if(!isNaN(set_channels[i])){
                this.channels[i] = +set_channels[i];
            }
            // console.log(this.channels)
        }

    }
    setAnimotor   = function(ClaSS){
        this.dom.style.display = 'block';
        this.size = this.channels[7]*4;
        console.log(this.size);
        this.resize()
        this.animotor =  setInterval(function(){ClaSS.Animation()},60000/this.channels[5]+10);
    }
    ChangeSpeed   = function(ClaSS){
        clearInterval(this.animotor);
        this.animotor =  setInterval(function(){ClaSS.Animation()},60000/this.channels[5]+10);
    }
    clearAnimotor = function(){
        this.dom.style.display = 'none';
        clearInterval(this.animotor);
    }
}

//Channels 1 2 3 --> inUSE ATM rgbw
class The_Matrix{
    dom = {}  //canvas ?
    channels = [255,0,0,0,0,0,0,0]; // r g b w 
    active   = false;

    constructor(element){
        this.dom = document.createElement('canvas');
        this.dom.id = 'The_matrix';
        this.dom.style.display = 'none';
        this.element = element;
        element.appendChild(this.dom);
        this.resize();
    }

    resize = function(){
        this.ctx                     = this.dom.getContext('2d');
        this.positionInfo            = this.element.getBoundingClientRect();
        this.dom.width               = this.positionInfo.width;
        this.dom.height              = this.positionInfo.height;
        this.ypos                    = Array(Math.floor(this.positionInfo.width / 20) + 1).fill(0);
    }

    set_Channels = function(set_channels){
        //todo check channels len
        for(var i = 0; i <set_channels.length; i++){
            if(i > this.channels.length){break;}
            if(!isNaN(set_channels[i])){
                this.channels[i] = +set_channels[i];
            }
        }
        console.log(this.channels);
    }
    Animation = function(){
        this.ctx.fillStyle = '#0001';
        this.ctx.fillRect(0, 0, this.dom.width, this.dom.height);
        var r = (this.channels[0]);
        var g = (this.channels[1]);
        var b = (this.channels[2]);
        var w = (this.channels[3]);
        if(w){
            r = (w + this.channels[0]/2.56)%256;
            g = (w + this.channels[1]/2.56)%256;
            b = (w + this.channels[2]/2.56)%256;
        }         
        this.ctx.fillStyle = 'rgb('+r+','+g+','+b+')';
        this.ctx.font = '15pt monospace';
        this.ypos.forEach((y, ind) => {
            const text = String.fromCharCode(Math.random() * 128);
            const x = ind * 20;
            this.ctx.fillText(text, x, y);
            if (y > 100 + Math.random() * 10000) this.ypos[ind] = 0;
            else this.ypos[ind] = y + 20;
        });
    }
    setAnimotor = function(ClaSS){
        this.dom.style.display = 'block';

        this.animotor =  setInterval(function(){ClaSS.Animation()},60000/this.channels[5]/10+10);
    }
    ChangeSpeed   = function(ClaSS){
        clearInterval(this.animotor);
        this.animotor =  setInterval(function(){ClaSS.Animation()},60000/this.channels[5]/10+10);
    }
    clearAnimotor = function(){
        this.dom.style.display = 'none';
        clearInterval(this.animotor);
    }
}
