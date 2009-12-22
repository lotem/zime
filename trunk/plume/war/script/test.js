// test.js

var TestFrontend = new Class({

    initialize: function () {
        Logger.debug("TestFrontend.initialize");
        this._backend = Backend.create();
        this._backend.loadSchemaList(this._loadSchemaListCallback);
    },

    updatePreedit: function (preeditText, selStart, selEnd) {
        Logger.debug("TestFrontend.updatePreedit: ", preeditText, selStart, selEnd);
    },

    updateCandidates: function (candidateList) {
        Logger.debug("TestFrontend.updateCandidates: ", candidateList);
    }, 

    commit: function (commitText) {
        Logger.debug("TestFrontend.commit: ", commitText);
    }, 

    submit: function () {
        Logger.debug("TestFrontend.submit");
    },

    displaySchemaList: function () {
        // TODO
    },

    createEngine: function () {
        // TODO
        // this._engine = new Engine(schema, this._backend);
    },
    
    input: function (text) {
        // TODO: foreach char in text:
        // e = new KeyEvent(keycode of char, mask);
        // this._engine.processKeyEvent(e);
    },

    _loadSchemaListCallback: function (schemaList) {
        var cont = $('<div id="schemaList" />');
        cont.append($('<p>SchemaList:</p>'));
        $.each(schemaList, function (i, item) {
            var p = $('<p />').text (item.schema + " - " + item.displayName);
            cont.append(p);
        });
        $("body").append(cont);
    }
    
});

Frontend.register(TestFrontend);

// on page load
$(function () {
    testAjax();
    testBootstrap();
});

function testAjax() {
    var cont = $('<div id="testAjax" />');
    cont.append($('<p>testAjax:</p>'));
    $.getJSON("sample.json", null, function (data) {
        $.each(data, function (i, item) {
            var p = $('<p />').text (item.schema + " - " + item.displayName);
            cont.append(p);
        });
        $("body").append(cont);
    });
}

function testBootstrap() {
    var t = Frontend.create();
}
