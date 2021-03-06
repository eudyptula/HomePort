
class BlockView {
    public static GetTimedBlockView(id: string, parentType : string, content : HTMLElement, onBeginDragCallback : Function, onDragCall : Function, onDropCallback : Function) {
        //Container element
        
        var output = document.createElement('div');
        output.id = id;
        output.setAttribute("data-timeline-type", parentType);

        output.classList.add('block');
        output.classList.add('timedBlock');
        output.classList.add('blockSpacing');
        output.classList.add('appended');

        //start of arrow 
        output.appendChild(UIHelper.CreateStartArrow(id + "_startArrow", new Array(), new Array("timedBlock")));

        //dynamic content element
        var contentWrap = document.createElement('div');
        contentWrap.id = id + "_content";
        contentWrap.classList.add("contentBackground");
        contentWrap.classList.add("blockContent");
        contentWrap.appendChild(content);
        output.appendChild(contentWrap);

        //drag area
        var drag = document.createElement('div')
        drag.id = id + "_handle";
        drag.classList.add('dragHere'); //not used... should probably be removed if you read this.
        output.appendChild(drag);

        //end of arrow (pointy end)
        output.appendChild(UIHelper.CreateEndArrow(id + "_endArrow", new Array("blockspacing"), new Array("timedBlock")));


        //Hook up events
        $(output).draggable({
            handle: "#" + id + "_handle",
            containment: "#wrapper",
            revert: "invalid",
            start: function (event: JQueryUI.DraggableEvent, ui: JQueryUI.DraggableEventUIParams) {
                console.log("START: drag of element id: " + ui.helper[0].id + ".");
                onBeginDragCallback(event, ui);
            },
            drag: function (event : JQueryUI.DraggableEvent, ui: JQueryUI.DraggableEventUIParams) {
                onDragCall(event, ui);
            }, 
            stop: function (event: JQueryUI.DraggableEvent, ui: JQueryUI.DraggableEventUIParams) {
                console.log("STOP: drag of element id: " + ui.helper[0].id + ".");
                onDropCallback(event, ui);
            }
        });

        return output;
    }

    public static GetBlockContentView(id, deviceID, serviceID, value) {
        var output = document.createElement('div');
        var device = document.createElement('div');
        device.id = id + "_contentDeviceID";
        device.textContent = deviceID;

        var service = document.createElement('Div');
        service.id = id + "_contentServiceID";
        service.textContent = serviceID + value;

        output.appendChild(device);
        output.appendChild(service);
        return output;
    } 

    public static GetNoneView(id: string, parentType: string, content: HTMLElement, onDragCallback: Function, onDropCallback: Function) {

    }

    public static GetStartView(id : string) {
        var output = document.createElement('div');
        output.id = id;
        output.style.display = "inline-block";
        output.appendChild(UIHelper.CreateEndArrow(id + "_endArrow", new Array(), new Array("startTriangle", "block")));
        return output;
    }
    

    public static GetTimedDropView(id: string, parentType : string, afterIndex : number, onDropCallbacks : Array<Function>) {
        //Container element
        var output = document.createElement('div');
        output.id = id;
        output.setAttribute("data-timeline-type", parentType);
        output.setAttribute("data-after-index", afterIndex.toString());

        output.classList.add('block');
        output.classList.add('timedBlock');
        output.classList.add('blockSpacing');
        output.classList.add('dropArrow');
        output.classList.add('appended');

        //start of arrow 
        output.appendChild(UIHelper.CreateStartArrow(id+ "_startArrow", new Array(), new Array("timedBlock", "dropspace")));

        //filler
        var middle = document.createElement("div");
        middle.textContent = "Drop here!"
        middle.classList.add("blockContent");
        middle.classList.add("dropSpaceBackground");
        output.appendChild(middle);

        //drop area

        var drop = document.createElement('div')
        drop.id = id +"_dropArea";
        drop.classList.add('dropHere');
        middle.appendChild(drop);

        //add handler to drop area
        $(drop).droppable({
            drop: function (event, ui) {
                onDropCallbacks.forEach((callback, undex, array) => {
                    callback(event, ui, output);
                });
            }
        });


        //end of arrow (pointy end)
        output.appendChild(UIHelper.CreateEndArrow(id + "_endArrow", new Array("blockspacing"), new Array("timedBlock", "dropspace")));
        return output;
    }
}

var BlockTypeEnum = {
    When: 'When',
    If: 'If',
    Then: 'Then',
    None: 'None'
}
