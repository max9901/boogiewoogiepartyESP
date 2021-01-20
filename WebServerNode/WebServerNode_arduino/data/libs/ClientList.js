class ClientListClass{
    dom = {};
    constructor(element){
        this.dom = document.createElement('div');
        this.dom.className = 'ClientList';
        element.appendChild(this.dom);
        this.initclients();
    }

    initclients = function() {
        this.dom.clients = {};
        this.dom.clients.list = [];
        this.dom.clients.container = document.createElement('div');
        addClass(this.dom.clients.container, 'clients-container');
        this.dom.appendChild(this.dom.clients.container);
    }

    Clients_reset = function(){
        for (var i = this.dom.clients.list.length; i > 0 ; i--) {
            console.log(this.dom.clients.list[i-1].id);
            this.dom.clients.list[i-1].remove();
            this.dom.clients.list.splice(i-1,1);    
        }
    }
    
    Clients_update = function(newclientlist,FlagforALL){
        for (var i = this.dom.clients.list.length; i > 0 ; i--) {
            console.log(this.dom.clients.list[i-1].id);
            this.dom.clients.list[i-1].remove();
            this.dom.clients.list.splice(i-1,1);    
        }
        if(FlagforALL){
            this.Client_Add("All");
        }
        for (var i = 0; i < newclientlist.length; i++) {
            this.Client_Add(newclientlist[i]);
        } 
    }

    Get_list_string = function(){
        var ClientlistString = ""
        // console.log(this.dom.clients.list.length)
        for (var i = 0; i < this.dom.clients.list.length; i++) {
            if(this.dom.clients.list[i].className == "client active"){
                // console.log(this.dom.clients.list[i].id);
                if(this.dom.clients.list[i].id == "All"){
                    return "All";
                }
                if(ClientlistString){
                    ClientlistString += ",";
                }
                ClientlistString += this.dom.clients.list[i].id
            }
        }
        return ClientlistString;
    }

    Client_Add = function(title) {
        var client = document.createElement('div');
        client.id = title;

        client.addEventListener('click', function() {
            if (client.className.includes('active')) {
                removeClass(client, 'active');
            }
            else{
                addClass(client, 'active');
            }
        });
        addClass(client, 'client');
        this.dom.clients.container.appendChild(client)
        this.dom.clients.list.push(client);
        client.appendChild(document.createTextNode(title));
        return client;
    }

}