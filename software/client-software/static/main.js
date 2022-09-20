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
      backgroundColor: 'rgb(255, 99, 132)',
      borderColor: 'rgb(255, 99, 132)',
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