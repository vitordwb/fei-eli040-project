const char MAIN_page[] PROGMEM = R"=====(
<!doctype html>
<html>
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Monitoramento de Energia</title>
    <script src = "https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.7.3/Chart.min.js"></script>
</head>
<body>
    <!--Primeira linha da pagina-->
    <div style="text-align:center;"><b>Corrente RMS, Potencia Aparente, Potencia Ativa e Consumo utilizando o ESP8266</b></div>
    <div class="chart-container": position: relative; height:350px; width:100%>
    <canvas id="Chart" width="400" height="400"></canvas>
    </div>
    <div>
    <table id="dataTable">
    <!--Cabecalho da tabela-->
    <tr><th>Horario</th><th>Corrente RMS (A)</th><th>Potencia Aparente (kVA)</th><th>Potencia Ativa (FP=0.8 - kW)</th><th>Consumo (FP=0.8 - kWh)</th></tr>
    </table>
    </div>
    <br>
    <br>

    <script>
        var CorrenteRMS = [];
        var PotenciaS = [];
        var PotenciaP = [];
        var ConsumoEE = [];
        var timeStamp = [];

        function showGraph() {
        var ctx = document.getElementById("Chart").getContext('2d');
        var Chart2 = new Chart(ctx, {
        type: 'line',
        data: {
            labels: timeStamp, // Bottom Labeling
            datasets: [{
            label: "Corrente RMS (A)",
            fill: false,
            backgroundColor: 'rgba( 243, 156, 18 , 1)', // Dot marker color
            borderColor: 'rgba( 243, 156, 18 , 1)',     // Graph Line Color
            data: CorrenteRMS,
        },
        {
            label: "Potencia Aparente (kVA)",
            fill: false,
            backgroundColor: 'rgba(156, 18, 243 , 1)', // Dot marker color
            borderColor: 'rgba(156, 18, 243 , 1)',     // Graph Line Color
            data: PotenciaS,
        },
        {
            label: "Potencia Ativa (FP=0.8 - kW)",
            fill: false,
            backgroundColor: 'rgba(204, 0, 0 , 1)', // Dot marker color
            borderColor: 'rgba(204, 0, 0 , 1)',     // Graph Line Color
            data: PotenciaP,
        },
        {
            label: "Consumo (FP=0.8 - kWh)",
            fill: false,
            backgroundColor: 'rgba(0, 51, 102 , 1)', // Dot marker color
            borderColor: 'rgba(0, 51, 102 , 1)',     // Graph Line Color
            data: ConsumoEE,
        }
            ],
        },
        options: {
        title: {
            display: true,
            text: "Dados aferidos em tempo real"
            },
            maintainAspectRatio: false,
            elements: {
                line: {
                tension: 0.5 // Smoothening (Curved) of data lines
                }
            },
        scales: {
            yAxes: [{
                ticks: {
                    beginAtZero:true
                }
            }]
        }
        }
        });
        }

        // On Page load show graphs
        window.onload = function() {
        console.log(new Date().toLocaleTimeString());
        };
        // Ajax script to get ADC voltage at every 5 Seconds
        // Read This tutorial https://circuits4you.com/2018/02/04/
        //   esp8266-ajax-update-part-of-web-page-without-refreshing/
        setInterval(function() {
        // Call a function repetatively with 5 Second interval
        getData();
        }, 5000);  // 5000mSeconds  update rate
        function getData() {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            // Push the data in array
            var time = new Date().toLocaleTimeString();
            var txt = this.responseText;
            var obj = JSON.parse(txt);  // Ref: https://www.w3schools.com/js/js_json_parse.asp
            CorrenteRMS.push(obj.Irms);
            PotenciaS.push(obj.potAparente);
            PotenciaP.push(obj.potAtiva);
            ConsumoEE.push(obj.Consumo);
            timeStamp.push(time);
            showGraph();  // Update Graphs

            // Update Data Table
            var table = document.getElementById("dataTable");
            var row = table.insertRow(1); // Add after headings
            var cell1 = row.insertCell(0);
            var cell2 = row.insertCell(1);
            var cell3 = row.insertCell(2);
            var cell4 = row.insertCell(3);
            var cell5 = row.insertCell(4);
            cell1.innerHTML = time;
            cell2.innerHTML = obj.Irms;
            cell3.innerHTML = obj.potAparente;
            cell4.innerHTML = obj.potAtiva;
            cell5.innerHTML = obj.Consumo;
        }
    };
        xhttp.open("GET", "readADC", true); // Handle readADC server on ESP8266
        xhttp.send();
    }

    </script>
</body>
</html>
)=====";