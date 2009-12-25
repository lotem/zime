// test.js

var TestFrontend = Class.extend(Frontend, {

    initialize: function () {
        Logger.debug("TestFrontend.initialize");
        this._backend = Backend.create();
        this._schemaList = [];
        var me = this;
        this._backend.loadSchemaList(function (schemaList) {
            var cont = $('<div id="schemaList" />');
            cont.append($('<b>schemaList:</b><hr />'));
            $.each(schemaList, function (i, item) {
                var p = $('<p />').text (item.schema + " - " + item.displayName);
                cont.append(p);
            });
            $("body").append(cont);
            me._schemaList = schemaList;
            if (schemaList.length > 0) {
                me.loadSchema(schemaList[0].schema);
            }
        });
    },

    updatePreedit: function (preeditText, selStart, selEnd) {
        Logger.debug("TestFrontend.updatePreedit: " + preeditText + "[" + selStart + ", " + selEnd + "]");
    },

    updateCandidates: function (candidateList) {
        Logger.debug("TestFrontend.updateCandidates: " + candidateList);
    }, 

    commit: function (commitText) {
        Logger.debug("TestFrontend.commit: " + commitText);
    }, 

    submit: function () {
        Logger.debug("TestFrontend.submit");
    },

    loadSchema: function (schemaName) {
        Logger.debug("TestFrontend.loadSchema: " + schemaName);
        var me = this;
        this._backend.loadConfig(schemaName, function (config) {
            schema = new Schema (schemaName, config);
            me.engine = new Engine(schema, me, me._backend);
            testAll(me);
        });
    },
    
    input: function (text) {
        // TODO: foreach char in text:
        // e = new KeyEvent(keycode of char, mask);
        // this.engine.processKeyEvent(e);
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
    cont.append($('<b>testAjax:</b><hr />'));
    $.getJSON("script/sample.json", null, function (data) {
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

function testAll(t) {
    Logger.debug("bootstrap completed");
    Logger.debug("active schema: " + t.engine.schema.schemaName);
    //testConfig(t);
    testSchema(t);
}

function testConfig(t) {
    var cont = $('<div id="testConfig" />');
    cont.append($('<b>keywords:</b><hr />'));
    var a = t.engine.schema.keywords;
    for (var k in a) {
        cont.append($('<span />').text (k + " --> " + a[k]));
        cont.append($('<br />'));
    }
    cont.append($('<b>fuzzyMap:</b><hr />'));
    var a = t.engine.schema.fuzzyMap;
    for (var k in a) {
        cont.append($('<span />').text (k + " --> " + a[k]));
        cont.append($('<br />'));
    }
    $("body").append(cont);
}

function testSchema(t) {
    var s = t.engine.schema;
    var cont = $('<div id="testSchema" />');
    cont.append($('<b>testSchema:</b><hr />'));
    cont.append($('<p />').text("parserName: " + s.getParserName()));
    cont.append($('<p />').text("punct:"));
    var ul = $('<ul />');
    $.each(s.getConfigList("Punct"), function (i, e) {
        ul.append($('<li />').text(e));
    });
    cont.append(ul);
    $("body").append(cont);
}

