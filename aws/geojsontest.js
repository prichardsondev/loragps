//test process for getting data from s3 in geojson format ->
//parsing to json array -> adding new coordiante to json array ->
//parsing back to geojson.

var GeoJSON = require('geojson');

var data = [
    { lat: 39.984, lng: -75.343 },
    { lat: 39.284, lng: -75.833 },
    { lat: 39.123, lng: -74.534 }
];

//create geojson object 
var geojson = GeoJSON.parse(data, { Point: ['lat', 'lng'] });

//parse geojson to json object
var json = JSON.parse(JSON.stringify(geojson, null, 2));

//create array of lat lng objects - gotta love map
let result = json.features.map(c =>
    ({ lat: c.geometry.coordinates[1], lng: c.geometry.coordinates[0] })
);

//append new lat lng object to array
var data2 = { lat: 43, lng: -70 };
result.push(data2);

//create new geojson object with new lat lng added
var geojson2 = GeoJSON.parse(result, { Point: ['lat', 'lng'] });
//shazam
printPretty(geojson2);

function printPretty(j) {
    console.log(JSON.stringify(j, null, 2));
}