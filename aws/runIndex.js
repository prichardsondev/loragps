let index = require('./index');

//test data for 'lambda proxy integration' body... Stright lambda integration
//just passes data you need not the whole kitchen sink like proxy.
//Hence the ugly json string literal below I made for this test file.

let event = {
    "body": "{\"payload_fields\" : {\"latitude\": \"100\",\"longitude\": \"100\"}}"
};

index.handler(event);