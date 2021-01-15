class ButtonboardClass{
    button_list = [];
    dom = {};
    constructor(element){
        this.dom = document.createElement('div');
        this.dom.className = 'ButtonBoard';
        element.appendChild(this.dom);
        this.initbuttons();
    }

    initbuttons = function() {
        this.dom.buttons = {};
        this.dom.buttons.list = [];
        this.dom.buttons.listlinked = [];
        this.dom.buttons.container = document.createElement('div');
        addClass(this.dom.buttons.container, 'buttons-container');
        this.dom.appendChild(this.dom.buttons.container);
    }
    
    addButton_unique = function(title,fnc) {
        var button = document.createElement('div');
        button.addEventListener('click', function() {
            if (button.className.includes('active')) {
                removeClass(button, 'active');
            }
            else{
                addClass(button, 'active');
            }
            fnc(button);
        });
        addClass(button, 'btn');
        this.dom.buttons.container.appendChild(button)
        this.dom.buttons.list.push(button);
        button.appendChild(document.createTextNode(title));
        return button;
    }

    addButton = function(title,fnc) {
        var button = document.createElement('div');
        var $this = this;
        button.addEventListener('click', function() {
            for (var i = 0; i < $this.dom.buttons.listlinked.length; i++) {
                removeClass($this.dom.buttons.listlinked[i], 'active');
            }
            addClass(button, 'active');
            fnc(button);
        });
        addClass(button, 'btn');
        this.dom.buttons.container.appendChild(button)
        this.dom.buttons.listlinked.push(button);
        this.dom.buttons.list.push(button);
        button.appendChild(document.createTextNode(title));
        return button;
    }
}

var buttonboard
function btn_stobo(strobo_button){
    if (strobo_button.className.includes('active')){
        console.log("strobo ON");
    }else{
        console.log("strobo off")
    }
}