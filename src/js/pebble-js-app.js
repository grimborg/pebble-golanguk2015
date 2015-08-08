Pebble.timelineSubscribe('talks',
    function () {
        console.log('Subscribed to talks');
    },
    function (errorString) {
        console.log('Error subscribing to talks: ' + errorString);
    }
);
