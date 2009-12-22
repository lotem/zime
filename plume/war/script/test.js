// test.js

var TestEngine = new Class({

    initialize: function () {
    }

});

$(function () {
    var e = new TestEngine ();
    
    // test ajax
    $.getJSON("sample.json", null, function (data) {
        $.each(data, function (i, item) {
            var p = $("<p></p>").text (item.displayName);
            $("#cont").append(p);
        });
    });

});
