//update profile, Bucket,  with your info
//push gps.json to your bucket before running.
//could modify code to handle missing gps.json
//file on first run...

"use strict";
const AWS = require("aws-sdk");
const GeoJSON = require('geojson'); 

//remove for aws
AWS.config.credentials = new AWS.SharedIniFileCredentials({
    profile: "default"
  });

AWS.config.update({ region: "us-east-1" });

exports.handler = async event => {
    const response = {
        statusCode: 0,
        body: ""
    };

    //get TTN payload_fields body
    const { latitude, longitude } = JSON.parse(event.body).payload_fields;
    console.log(latitude,longitude);

    //check for truthiness (! null, 0, NaN, ...)
    if( !latitude || !longitude) return;

    //new coords - will add to geojson object below \v/
    const coords = {
        lat:parseFloat(latitude),
        lng:parseFloat(longitude)
    }

    var s3 = new AWS.S3();

    var params = {
        Bucket: 'bucket-name',
        Key: 'gps.json',
    };

    //get gps.json from s3 and await it's return
    let json;
    try {
        json = await s3.getObject(params).promise();
    } catch (err) {
        console.log(err);
        return err;
    } 
    
    //convert returned buffer to json
    let data = JSON.parse(json.Body.toString());
    
    //parse to array of json objects - I love map
    let result = data.features.map(c =>
        ({ lat: c.geometry.coordinates[1], lng: c.geometry.coordinates[0] })
    );

    //check for duplicate entry (gps not moving)
    if(duplicateCoords(result,coords)) return;

    //add new coordinate to array
    result.push(coords);

    //turn back into geojson object
    var geojson = GeoJSON.parse(result, { Point: ['lat', 'lng'] });

    //prepare params for putObject - other params already set above
    params.Body = JSON.stringify(geojson);
    params.ContentType = 'application/json; charset=utf-8';
    
    //push new geojson object to s3 and await... 
    let newjson;
    try {
        newjson = await s3.putObject(params).promise();
    } catch (err) {
        console.log(err);
        return err;
    }

    try {
        response.body = JSON.stringify(newjson);
        response.statusCode = 201;
    } catch (err) {
        response.body = JSON.stringify("Something went horribly wrong....");
        response.statusCode = 403;
    }

    return response;
};

function duplicateCoords(data,coords){
    return data.some(c => JSON.stringify(c) 
        === JSON.stringify(coords));      
}