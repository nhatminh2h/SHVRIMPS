let HZ = 30;
let feed_screen = document.getElementById("feed");
let dat;
let t = 0;
let maxDataPoints = 50;

const labels = [];

  const data = {
    labels: labels,
    datasets: [{
      label: 'channel 1',
      backgroundColor: 'rgb(230,214,144)',
      borderColor: 'rgb(230,214,144)',
      data: [],
    }, {
      label: 'channel 2',
      backgroundColor: 'rgb(155,017,030)',
      borderColor: 'rgb(155,017,030)',
      data: [],
    }, {
      label: 'channel 3',
      backgroundColor: 'rgb(071,064,046)',
      borderColor: 'rgb(071,064,046)',
      data: [],
    }, {
      label: 'channel 4',
      backgroundColor: 'rgb(044,085,069)',
      borderColor: 'rgb(044,085,069)',
      data: [],
    }, {
      label: 'channel 5',
      backgroundColor: 'rgb(053,104,045)',
      borderColor: 'rgb(053,104,045)',
      data: [],
    }, {
      label: 'channel 6',
      backgroundColor: 'rgb(255, 0, 0)',
      borderColor: 'rgb(255, 0, 0)',
      data: [],
    },{
      label: 'channel 7',
      backgroundColor: 'rgb(0, 255, 0)',
      borderColor: 'rgb(0, 255, 0)',
      data: [],
    }, {
      label: 'channel 8',
      backgroundColor: 'rgb(0,0,255)',
      borderColor: 'rgb(0,0,255)',
      data: [],
    }]
  };

  const config = {
    type: 'line',
    data: data, 
    options: {}
  };

  const myChart = new Chart(
    document.getElementById('myChart'),
    config
  );

function encodeToBase64(img)
{
    let byteArray = new Uint8Array(img);
    let stringByteArray = "";
    for (var i = 0; i < byteArray.length; ++i)
    {
        stringByteArray += String.fromCharCode(byteArray[i]);
    }
    return btoa(stringByteArray);
}

function poll_data()
{
    socket.emit("get_data", {});
}

function update(data)
{
    let new_image = data["feed"];
    dat = data;
    feed_screen.src = "data:image/jpeg;base64," + new_image;
}

function plot(data)
{
    measurements = data['voltage_measurements']; 
    for (let i = 0; i < measurements.length; ++i)
    {
        myChart.data.datasets[0].data.push(measurements[i]);
        myChart.data.labels.push(t)
    }


    //if (myChart.data.datasets[0].data.length == maxDataPoints)
    //{
    //   myChart.data.datasets[0].data.shift();
    //    myChart.data.labels.shift();
    //}

    t += 1;
    myChart.update();
}

var socket = io();

//socket.on('connect', function() {});
socket.on("data", update);
socket.on("voltage", plot);

window.setInterval(poll_data, 1000 / HZ);