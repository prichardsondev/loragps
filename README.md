# loragps

### basic flow: 
    feather (arduino) -> TTN -> TTN http integration -> API Gateway -> Lambda -> S3 ->
    http file maps geojson data written to s3

### 3 parts

#### arduino
    sends gps data to TTN - The Things Network

#### aws 
    cd aws
    npm i

    you can test your aws code locally by calling runIndex.js
    see notes in index.js before running runIndex.js

    once working post to your lambda.
    create separate project for index.js.
    npm init -y
    npm install geojson
    zip index.js and node_modules
    upload to s3 bucket
    import into lambda

    runs some tests in Lambda console to make sure it working

    you'll need an api gateway -> single post method with
    LAMBDA_PROXY integration

    add The Things Network http integration to your api gateway endpoint

#### web
    add all three files to s3 bucket or wherever you choose

