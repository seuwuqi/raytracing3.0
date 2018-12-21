// 基于准备好的dom，初始化echarts实例
// var button1 = document.getElementById("pathloss");
// var button2 = document.getElementById("powerdelay");
// var button3 = document.getElementById("meandelay");
// var button4 = document.getElementById("dimpowerdelay");

var pathLossChart = echarts.init(document.getElementById("pathloss"),'light');
var powerDelayChart = echarts.init(document.getElementById("powerDelay",));
var dopplerChart = echarts.init(document.getElementById("doppler"),'light');
var speedChart = echarts.init(document.getElementById("speedDistance"),'light');
// button1.onclick = function(ev){
//     loadPathLoss();
// }
// button2.onclick = function(ev){
//     loadPowerDelayProfile();
// }
// button4.onclick = function (ev) {
//     loadDimenPowerDelay();
// };
// button3.onclick = function (ev) {
//     console.log("b3");
//     loadbutton2();
// };



function processTimeSpeed(time,distance,tx_v,rx_v) {
    var data1 = [];
    var data2 = [];
    var data3 = [];
    for(var i = 0; i < time.length; i++){
        data1.push([time[i],tx_v[i]]);
        data2.push([time[i],rx_v[i]]);
        data3.push([time[i],distance[i]]);
        console.log(data1);
        console.log(data2);
        console.log(data3);
    }
    loadSpeedDistance(data1,data2,data3);
}

function processResult(data){
    var paths = data.paths;
    var pairs = [];
    var dopplerPairs = [];
    var map =  new Map();
    for (var p in paths){
        var path = paths[p];
        map.set(path.pathloss, path);
    }

    paths = Array.from(map.values());
    // console.log(paths);

    for (var p in paths){
        var path = paths[p];
        var pair = {
            "timeDelay" : path.timeDelay,
            "pathLoss" : path.pathloss
        };
        var dopplerPair =  {
            "dopplerShift" : path.dopplerShift,
            "pathLoss" : path.pathloss
        };
        dopplerPairs.push(dopplerPair);
        pairs.push(pair);

    }
    pairs.sort(function (a,b) {
        return a.timeDelay - b.timeDelay;
    });
    dopplerPairs.sort(
        function (a,b) {
            return a.dopplerShift - b.dopplerShift;
        }
    );
    var timeDelay = [];
    var pathLoss = [];
    for (var i = 0; i < pairs.length; i++){
        timeDelay.push(pairs[i].timeDelay - pairs[0].timeDelay);
        pathLoss.push(pairs[i].pathLoss);
    }

    var locations = [];
    for (var i = 0; i < timeDelay.length; i++){
        var location = [];
        location.push(timeDelay[i]);
        location.push(pathLoss[i]);
        locations.push(location);
    }

    var dopplerShift = [];
    var pathLoss2 = [];
    for (var i = 0; i < dopplerPairs.length; i++){
        dopplerShift.push(dopplerPairs[i].dopplerShift);
        pathLoss2.push(dopplerPairs[i].pathLoss);
    }
    // dopplerShift = unique(dopplerShift);
    // pathLoss2 = unique(pathLoss2);
    console.log(dopplerShift.length);
    console.log(pathLoss2.length);

    var locations2 = [];
    for (var i = 0; i < dopplerShift.length; i++){
        var location = [];
        location.push(dopplerShift[i]);
        location.push(pathLoss2[i]);
        locations2.push(location);
    }
    console.log(locations2);
    loadPowerDelayProfile(locations,data.averageDelay,data.rms);

    loadDopplerShift(locations2);

}
function unique(array) {
    return Array.from(new Set(array));
}
// loadPathLoss([1, 2, 3, 4, 5, 6], [-50, -80, -100, -103, -99, -98]);


function processDynamicDoppler(time,paths) {
    var data = [];
    for(var i = 0; i < time.length; i++){
        var path = paths[i];
        for(var j = 0; j < path.length; j++){
            data.push([time[i],path[j].dopplerShift, path[j].pathloss]);
        }
    }
    console.log(data);
    loadDopplerDynamic(data);

}

function processDynamicDelay(time, paths,averageDelay, rms) {
    var data = [];
    var data2 = [];
    var data3 = [];
    for(var i = 0; i < time.length; i++){
        var path = paths[i];
        for(var j = 0; j < path.length; j++){
            data.push([time[i], path[j].timeDelay, path[j].pathloss]);
            data2.push([time[i], averageDelay[i]]);
            data3.push([time[i], averageDelay[i] + 2*rms[i]]);
        }
    }
    // console.log(data);
    loadPowerDelayDynamic(data,data2,data3);
}


function loadSpeedDistance(data1,data2,data3) {

    option = {
        // Make gradient line here

        legend: {
            data:['tx速度', 'rx速度', '距离']
        },
        xAxis: [{
            type: "value",
            name: '时间 单位：s',
            nameLocation: 'center',
            nameGap:25
        }, {
            name: '时间 单位：s',
            nameLocation: 'center',
            nameGap:25,
            type: "value",
            gridIndex: 1
        }],
        yAxis: [{
            name: '速度 单位：m/s',
            nameLocation: 'center',
            nameGap:40,
            type: "value",
            splitLine: {show: false}
        }, {
            name: '距离 单位：m',
            nameLocation: 'center',
            nameGap:40,
            type: "value",
            splitLine: {show: false},
            gridIndex: 1
        }],
        grid: [{
            bottom: '60%'
        }, {
            top: '60%'
        }],
        series: [{
            name:'tx速度',
            type: 'line',
            showSymbol: false,
            data: data1,
            smooth: true
        }, {
            name:'rx速度',
            type: 'line',
            showSymbol: false,
            data: data2,
            smooth: true
        }, {
            name: '距离',
            type: 'line',
            showSymbol: false,
            data: data3,
            xAxisIndex: 1,
            yAxisIndex: 1,
            smooth: true
        }]
    };
    speedChart.setOption(option);
}


function loadDopplerDynamic(data){
    var option = {
        // title: {
        //     text: 'Dispersion of house price based on the area',
        //     left: 'center',
        //     top: 0
        // },
        visualMap: {
            min: -200,
            max: -70,
            dimension: 2,
            orient: 'vertical',
            right: 10,
            top: 'center',
            text: ['HIGH', 'LOW'],
            calculable: true,
            inRange: {
                // colorAlpha : 0.3,
                color: ['#313695', '#4575b4', '#74add1', '#abd9e9', '#e0f3f8', '#ffffbf', '#fee090', '#fdae61', '#f46d43', '#d73027', '#a50026']
            }
        },
        tooltip: {
            trigger: 'item',
            axisPointer: {
                type: 'cross'
            }
        },
        xAxis: [{
            type: 'value',
            name: '时间 单位：s',
            nameLocation: 'center',
            nameGap:25
        }],
        yAxis: [{
            type: 'value',
            name: '多普勒频移 单位:Hz',
            nameLocation: 'center',
            nameGap :40
        }],
        toolbox: {
            show: true,
            feature: {
                mark: {
                    show: true
                },
                dataView: {
                    show: true,
                    readOnly: false
                },
                magicType: {
                    show: true,
                    type: ['line', 'bar']
                },
                restore: {
                    show: true
                },
                saveAsImage: {
                    show: true
                }
            }
        },
        series: [{
            name: 'price-area',
            type: 'scatter',
            symbolSize: 10,
            // itemStyle: {
            //     normal: {
            //         borderWidth: 0.2,
            //         borderColor: '#fff'
            //     }
            // },
            data: data
        }]
    };

    dopplerChart.setOption(option);
}


function loadPowerDelayDynamic(data,data2,data3) {

    var option = {
        // title: {
        //     text: 'Dispersion of house price based on the area',
        //     left: 'center',
        //     top: 0
        // },
        visualMap: {
            min: -200,
            max: -70,
            dimension: 2,
            orient: 'vertical',
            right: 10,
            top: 'center',
            text: ['HIGH', 'LOW'],
            calculable: true,
            inRange: {
                // colorAlpha : 0.3,
                color: ['#313695', '#4575b4', '#74add1', '#abd9e9', '#e0f3f8', '#ffffbf', '#fee090', '#fdae61', '#f46d43', '#d73027', '#a50026']
            }
        },

        legend :{
            data:['平均附加时延']
        },
        tooltip: {
            trigger: 'item',
            axisPointer: {
                type: 'cross'
            }
        },
        xAxis: [{
            type: 'value',
            name:'时间 单位：s',
            nameLocation:'center',
            nameGap : 25
        }],
        yAxis: [{
            type: 'value',
            name:'附加时延 单位：μs',
            nameLocation:'center',
            nameGap : 40
        }],
        toolbox: {
            show: true,
            feature: {
                mark: {
                    show: true
                },
                dataView: {
                    show: true,
                    readOnly: false
                },
                magicType: {
                    show: true,
                    type: ['line', 'bar']
                },
                restore: {
                    show: true
                },
                saveAsImage: {
                    show: true
                }
            }
        },
        series: [{
            name: 'delay',
            type: 'scatter',
            symbolSize: 10,
            data: data
        // },{
        //     name:'averageDelay',
        //     type:'line',
        //     data:data2
        },{
            name:'平均附加时延',
            type:'line',
            data: data3
        }

        ]
    };

    powerDelayChart.setOption(option);
}


function loadDopplerShift(locations) {
    option = {
        xAxis: {
            name:'多普勒频移 单位：Hz',
            nameLocation :'center',
            type: 'value',
            nameGap : 25
        },
        yAxis: {
            type: 'value',
            name: '接收功率 单位：dBw',
            nameLocation :'center',
            nameGap : 40
        },
        series: [{
            data: locations,
            type: 'scatter'
        }],
        toolbox: {
            show: true,
            feature: {
                mark: {
                    show: true
                },
                dataView: {
                    show: true,
                    readOnly: false
                },
                magicType: {
                    show: true,
                    type: ['scatter', 'line','bar']
                },
                saveAsImage: {
                    show: true
                }
            }
        }
    };
    dopplerChart.setOption(option);
}

function loadPowerDelayProfile(locations,averageDelay, rms) {
    console.log(averageDelay);
    console.log(rms);
    option = {
        xAxis: {
            name:'相对时延 单位：μs',
            type: 'value',
            nameLocation :'center',
            nameGap : 25
        },
        yAxis: {
            type: 'value',
            name: '接收功率 单位：dBw',
            nameLocation :'center',
            nameGap : 40
        },
        series: [{
            data: locations,
            type: 'scatter',
            markLine: {
                data:[
                    {
                        // name:'平均附加时延',
                        xAxis:averageDelay
                    }
                ]
            },
            markArea: {
                data:[
                    [
                        {
                            // name:'平均附加时延和rms时延扩展',
                            nameLocation: 'center',
                            xAxis:averageDelay
                        },
                        {
                            xAxis:averageDelay+rms*2
                        }
                    ]
                ]
            }
        }],
        toolbox: {
            show: true,
            feature: {
                mark: {
                    show: true
                },
                dataView: {
                    show: true,
                    readOnly: false
                },
                magicType: {
                    show: true,
                    type: ['scatter', 'line','bar']
                },
                saveAsImage: {
                    show: true
                }
            }
        }
    };

    powerDelayChart.setOption(option);
}

function loadPL(locations) {
    option = {
        xAxis: {
            name:'时间 单位：s',
            type: 'value',
            nameLocation : 'center',
            nameGap : 25
        },
        yAxis: {
            type: 'value',
            name: '接收功率 单位:dBw',
            nameLocation : 'center',
            nameGap : 40
        },
        series: [{
            data: locations,
            type: 'line'
        }],
        toolbox: {
            show: true,
            feature: {
                mark: {
                    show: true
                },
                dataView: {
                    show: true,
                    readOnly: false
                },
                magicType: {
                    show: true,
                    type: ['scatter', 'line','bar']
                },
                saveAsImage: {
                    show: true
                }
            }
        }
    };

    pathLossChart.setOption(option);

}

function loadbutton2() {
    option = {
        title: {
            text: '平均附加时延',
            subtext: '附加时延'
        },
        tooltip: {},
        backgroundColor: '#fff',
        visualMap: {
            show: false,
            dimension: 2,
            min: -1,
            max: 1,
            inRange: {
                color: ['#313695', '#4575b4', '#74add1', '#abd9e9', '#e0f3f8', '#ffffbf', '#fee090', '#fdae61', '#f46d43', '#d73027', '#a50026']
            }
        },
        xAxis3D: {
            type: 'value'
        },
        yAxis3D: {
            type: 'value'
        },
        zAxis3D: {
            type: 'value'
        },
        grid3D: {
            viewControl: {
                // projection: 'orthographic'
            }
        },
        series: [{
            type: 'surface',
            wireframe: {
                // show: false
            },
            equation: {
                x: {
                    step: 0.05
                },
                y: {
                    step: 0.05
                },
                z: function (x, y) {
                    if (Math.abs(x) < 0.1 && Math.abs(y) < 0.1) {
                        return '-';
                    }
                    return Math.sin(x * Math.PI) * Math.sin(y * Math.PI);
                }
            }
        }]
    };
    myChart.setOption(option);
}

function loadDimenPowerDelay() {
    myChart.setOption({
        title: {
            text: '功率时延空间分布',
            subtext: '功率时延空间分布'
        },
        grid3D: {},
        xAxis3D: {},
        yAxis3D: {},
        zAxis3D: {},
        series: [{
            type: 'scatter3D',
            symbolSize: 1,
            data: [
                [-1, -1, -1],
                [0, 0, 0],
                [1, 1, 1]
            ],
            itemStyle: {
                opacity: 1
            }
        }]
    });
}



function loadPathLoss(pathLossOrder, pathLoss) {
    // 指定图表的配置项和数据
    option = {
        title: {
            // text: 'Ray-Tracing 路径损耗',
            // subtext: '路径损耗分布'
        },
        tooltip: {
            trigger: 'axis'
        },
        legend: {
            data: ['路径损耗']
        },
        toolbox: {
            show: true,
            feature: {
                mark: {
                    show: true
                },
                dataView: {
                    show: true,
                    readOnly: false
                },
                magicType: {
                    show: true,
                    type: ['line', 'bar']
                },
                restore: {
                    show: true
                },
                saveAsImage: {
                    show: true
                }
            }
        },
        calculable: true,
        xAxis: [{
            type: 'category',
            data: pathLossOrder
        }],
        yAxis: [{
            type: 'value'
        }],
        series: [{
            name: '路径损耗/dB',
            type: 'bar',
            data: pathLoss,
            markPoint: {
                data: [{
                    type: 'max',
                    name: '最大值'
                }, {
                    type: 'min',
                    name: '最小值'
                }]
            },
            markLine: {
                data: [{
                    type: 'average',
                    name: '平均值'
                }]
            }
        }]
    };
    // 使用刚指定的配置项和数据显示图表。
    passLossChart.setOption(option);
}