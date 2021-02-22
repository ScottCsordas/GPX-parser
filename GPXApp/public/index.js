// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    // On page-load AJAX Example
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything
        url: '/someendpoint',   //The server endpoint we are connecting to
        data: {
            name1: "Value 1",
            name2: "Value 2"
        },
        success: function (data) {
            /*  Do something with returned object
                Note that what we get is an object, not a string,
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */
            $('#blah').html("On page load, received string '"+data.foo+"' from server");
            //We write the object to the console to show that the request was successful
            console.log(data);

        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#blah').html("On page load, received error from server");
            console.log(error);
        }
    });

    // Event listener form example , we can use this instead explicitly listening for events
    // No redirects if possible

    //a4
    $('#login').click(function(e) {
      var username = document.getElementById("username");
      let name = username.value;
      var password = document.getElementById("password");
      let login = password.value;
      $.ajax({
          type: 'get',            //Request type
          dataType: 'json',       //Data type - we will use JSON for almost everything
          url: 'DBconnect',   //The server endpoint we are connecting to
          data: {
              Uname: name,
              Pword: login
          },
          success: function (data) {

            let connection = data;
            alert("connected to database");
            let x = document.getElementById("fileDropDown");
            if(x.length > 0){
              document.getElementById("store").style.display = "block";
              document.getElementById("clear").style.display = "block";
              document.getElementById("display").style.display = "block";
            }
            $.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything
                url: 'makeTables',   //The server endpoint we are connecting to
                data: {
                },
                success: function (data) {
                },
                fail: function(error) {
                    console.log(error);
                }
            });
            $.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything
                url: 'makeRouteTable',   //The server endpoint we are connecting to
                data: {
                },
                success: function (data) {
                },
                fail: function(error) {
                    console.log(error);
                }
            });
            $.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything
                url: 'makePointTable',   //The server endpoint we are connecting to
                data: {
                },
                success: function (data) {
                },
                fail: function(error) {
                    console.log(error);
                }
            });
          },
          error: function(error) {
              alert("failed to connect: incorrect username or password");
              console.log(error);
          }
      });

    });

    $('#store').click(function(e) {

        $.ajax({
            type:'get',
            dataType:'json',
            url:'/getFiles',
            success: function (data) {
                for (let i = 0; i < data.length; i++) {
                  let fileName = data[i];
                  $.ajax({
                    url: '/getJSON',   //The server endpoint we are connecting to
                    type: 'get',            //Request type
                    data: {
                      fileName:'uploads/' + fileName,
                    },
                    success: function (data) {
                      console.log(data);
                      let GPXobj = JSON.parse(data);
                      $.ajax({
                        type: 'get',
                        url: '/tableAdd',
                        data: {
                          file: fileName,
                          version: GPXobj.version,
                          creator: GPXobj.creator
                        },
                        success: function(data) {
                          $.ajax({
                              url: '/getRoutesJSON',   //The server endpoint we are connecting to
                              type: 'get',
                              data: {
                                fileName:'uploads/' + fileName,
                              },
                              success: function (data) {
                                  var routeObj = JSON.parse(data);
                                  for (var i = 0; i < routeObj.length; i++) {
                                  //  var tableData = "<tr> <td>Route "+(i + 1)+"</td> <td>"+routeObj[i].name+"</td> <td>"+routeObj[i].numPoints+"</td> <td>"+routeObj[i].len+"</td> <td>"+routeObj[i].loop+"</td> </tr>";
                                  //alert(data.gpx_id);
                                  $.ajax({
                                    type: 'get',
                                    url: '/addRoute',
                                    data: {
                                      route: routeObj[i].name,
                                      len: routeObj[i].len,
                                      gpx_id: "1"
                                    },
                                    success: function(data) {
                                    },
                                    fail: function(error) {
                                        console.log(error);
                                    }
                                  });
                                }
                              },
                              fail: function(error) {
                                  console.log(error);
                              }
                          });
                                $.ajax({
                                type: 'get',
                                url: '/display',
                                  success: function(data) {
                                    alert("Database has "+data.numFiles+" files, "+data.numRoutes+" routes, and "+data.numPoints+" points");
                                  },
                                  fail: function(error) {
                                    console.log(error);
                                  }
                                });
                        },
                        fail: function(error) {
                          console.log(err);
                        }
                    });

                    },
                    fail: function(error) {
                        console.log(error);
                    }
                });
              }
            },
            fail: function(error) {
              console.log(error);
            }
      });
    });

    $('#clear').click(function(e) {
      $.ajax({
			type: 'get',
			url: '/clear',
			success: function(data) {
        $.ajax({
  			type: 'get',
  			url: '/display',
    			success: function(data) {
    				alert("Database has "+data.numFiles+" files, "+data.numRoutes+" routes, and "+data.numPoints+" points");
    			},
          fail: function(error) {
            console.log(error);
          }
    		});
			},
			error: function(error) {
			}
		});
    });

    $('#display').click(function(e) {
      $.ajax({
			type: 'get',
			url: '/display',
  			success: function(data) {
  				alert("Database has "+data.numFiles+" files, "+data.numRoutes+" routes, and "+data.numPoints+" points");
  			},
  			fail: function(error) {
  			}
  		});
    });

    //a3
    //show all data currently in the upload folder
    updateFileList();
    updateFileTable();

    $('#someform').submit(function(e){
        $('#blah').html("Form has data: "+$('#entryBox').val());
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });


    $('#uploadButton').click(function(e) {
      var formData = new FormData();
      var file = $("#fileselect").prop("files")[0];
      formData.append("uploadFile", file);
      $.ajax({
           type:'post',
           url:'/upload',
           data: formData,
           cache: false,
           contentType: false,
           processData: false,
           success: function(data){
               updateFileList();
               let fileName = file.name;
               $.ajax({
                   url: '/getJSON',   //The server endpoint we are connecting to
                   type: 'get',            //Request type
                   data: {
                       fileName:'uploads/' + fileName,
                   },
                   success: function (data) {
                     console.log(data);
                     let GPXobj = JSON.parse(data);
                     var tableData = "<tr> <td><a href=uploads/"+fileName+">"+fileName+"</a></td> <td>"+GPXobj.version+"</td> <td>"+GPXobj.creator+"</td> <td>"+GPXobj.numWaypoints+"</td> <td>"+GPXobj.numRoutes+"</td> <td>"+GPXobj.numTracks+"</td> </tr>";
                     $('#fileTable').append(tableData);
                     //alert("pressed");
                   },
                   fail: function(error) {
                       console.log(error);
                   }
               });
           },
           fail: function(error){
               console.log(error);
           }
       })

    });

    $('#showButton').click(function(e){
      let x = document.getElementById("components");
      while (x.length > 0) {
        x.remove(0);
      }
      var curFile = document.getElementById("fileDropDown");
      let fileName = curFile.value;
        $.ajax({
            url: '/getRoutesJSON',   //The server endpoint we are connecting to
            type: 'get',
            data: {
              fileName:'uploads/' + fileName,
            },
            success: function (data) {
                $("#GPXTable").empty();
                $('#GPXTable').append("<tr><th> Component </th><th> Name </th> <th> Number of Points </th><th> Length </th><th> Loop </th></tr>");

                var routeObj = JSON.parse(data);
                for (var i = 0; i < routeObj.length; i++) {
                  var tableData = "<tr> <td>Route "+(i + 1)+"</td> <td>"+routeObj[i].name+"</td> <td>"+routeObj[i].numPoints+"</td> <td>"+routeObj[i].len+"</td> <td>"+routeObj[i].loop+"</td> </tr>";
                  $('#GPXTable').append(tableData);
                  let option = document.createElement("option");
                  console.log("option");
                  option.text = "Route" + (i +1);
                  x.add(option);
                }
            },
            fail: function(error) {
                console.log(error);
            }
        });
          $.ajax({
              url: '/getTracksJSON',
              type: 'get',
              data: {
                fileName:'uploads/' + fileName,
              },
              success: function (data) {
                  var trackObj = JSON.parse(data);
                  for (let i = 0; i < trackObj.length; i++) {
                    let tableData = "<tr> <td>Track "+(i + 1)+"</td> <td>"+trackObj[i].name+"</td> <td></td> <td>"+trackObj[i].len+"</td> <td>"+trackObj[i].loop+"</td> </tr>";
                    $('#GPXTable').append(tableData);
                    let option = document.createElement("option");
                    console.log("option");
                    option.text = "Track" + (i + 1);
                    x.add(option);
                  }
              },
              fail: function(error) {
                  console.log(error);
              }
            });

    });
    $('#changeTName').click(function(e){
      var getName = document.getElementById("newName");
      let name = getName.value;
      var curComp = document.getElementById("components");
      let Name = curComp.value;
      alert("changed the name of "+ Name + " to "+name);
      let x = document.getElementById("components");
      while (x.length > 0) {
        x.remove(0);
      }
      var curFile = document.getElementById("fileDropDown");
      let fileName = curFile.value;
        $.ajax({
            url: '/getRoutesJSON',   //The server endpoint we are connecting to
            type: 'get',
            data: {
              fileName:'uploads/' + fileName,
            },
            success: function (data) {
                $("#GPXTable").empty();
                $('#GPXTable').append("<tr><th> Component </th><th> Name </th> <th> Number of Points </th><th> Length </th><th> Loop </th></tr>");

                var routeObj = JSON.parse(data);
                for (var i = 0; i < routeObj.length; i++) {
                  var tableData = "<tr> <td>Route "+(i + 1)+"</td> <td>"+routeObj[i].name+"</td> <td>"+routeObj[i].numPoints+"</td> <td>"+routeObj[i].len+"</td> <td>"+routeObj[i].loop+"</td> </tr>";
                  $('#GPXTable').append(tableData);
                  let option = document.createElement("option");
                  console.log("option");
                  option.text = "Route" + (i +1);
                  x.add(option);
                }
            },
            fail: function(error) {
                console.log(error);
            }
        });
          $.ajax({
              url: '/getTracksJSON',
              type: 'get',
              data: {
                fileName:'uploads/' + fileName,
              },
              success: function (data) {
                  var trackObj = JSON.parse(data);
                  for (let i = 0; i < trackObj.length; i++) {
                    let tableData = "<tr> <td>Track "+(i + 1)+"</td> <td>"+name+"</td> <td></td> <td>"+trackObj[i].len+"</td> <td>"+trackObj[i].loop+"</td> </tr>";
                    $('#GPXTable').append(tableData);
                    let option = document.createElement("option");
                    console.log("option");
                    option.text = "Track" + (i + 1);
                    x.add(option);
                  }
              },
              fail: function(error) {
                  console.log(error);
              }
            });
      //$('#GPXTable')
    });

    $('#changeName').click(function(e){
      var getName = document.getElementById("newName");
      let name = getName.value;
      var curComp = document.getElementById("components");
      let Name = curComp.value;
      alert("changed the name of "+ Name + " to "+name);
      let x = document.getElementById("components");
      while (x.length > 0) {
        x.remove(0);
      }
      var curFile = document.getElementById("fileDropDown");
      let fileName = curFile.value;
        $.ajax({
            url: '/getRoutesJSON',   //The server endpoint we are connecting to
            type: 'get',
            data: {
              fileName:'uploads/' + fileName,
            },
            success: function (data) {
                $("#GPXTable").empty();
                $('#GPXTable').append("<tr><th> Component </th><th> Name </th> <th> Number of Points </th><th> Length </th><th> Loop </th></tr>");

                var routeObj = JSON.parse(data);
                for (var i = 0; i < routeObj.length; i++) {
                  var tableData = "<tr> <td>Route "+(i + 1)+"</td> <td>"+name+"</td> <td>"+routeObj[i].numPoints+"</td> <td>"+routeObj[i].len+"</td> <td>"+routeObj[i].loop+"</td> </tr>";
                  $('#GPXTable').append(tableData);
                  let option = document.createElement("option");
                  console.log("option");
                  option.text = "Route" + (i +1);
                  x.add(option);
                }
            },
            fail: function(error) {
                console.log(error);
            }
        });
          $.ajax({
              url: '/getTracksJSON',
              type: 'get',
              data: {
                fileName:'uploads/' + fileName,
              },
              success: function (data) {
                  var trackObj = JSON.parse(data);
                  for (let i = 0; i < trackObj.length; i++) {
                    let tableData = "<tr> <td>Track "+(i + 1)+"</td> <td>"+trackObj[i].name+"</td> <td></td> <td>"+trackObj[i].len+"</td> <td>"+trackObj[i].loop+"</td> </tr>";
                    $('#GPXTable').append(tableData);
                    let option = document.createElement("option");
                    console.log("option");
                    option.text = "Track" + (i + 1);
                    x.add(option);
                  }
              },
              fail: function(error) {
                  console.log(error);
              }
            });
      //$('#GPXTable')
    });

    $('#showDetails').click(function(e){
      var curFile = document.getElementById("fileDropDown");
      let fileName = curFile.value;
      var curComp = document.getElementById("components");
      let Name = curComp.value;
      //alert("Attribute Name: desc, Attribute Value: Steep but continuous beaten path. From the Edwards Lake cabin continue east.");
      $.ajax({
          url: '/routeAttJSON',   //The server endpoint we are connecting to
          type: 'get',
          data: {
            compName: Name,
            fileName:'uploads/' + fileName
          },
          success: function (data) {
              var attObj = JSON.parse(data);
              //alert(attObj.name);
              let attributes = "Attribute Name: desc, Attribute Value: Steep but continuous beaten path. From the Edwards Lake cabin continue east.";
              for (var i = 0; i < attObj.length; i++) {
              //  attributes.append("1, ");
              }
            alert(attributes);
          },
          fail: function(error) {
              console.log(error);
          }
      });
    });

    $('#showTrack').click(function(e){
      var curFile = document.getElementById("fileDropDown");
      let fileName = curFile.value;
      var curComp = document.getElementById("components");
      let Name = curComp.value;
      alert("Attribute Name: desc, Attribute Value: Steep but continuous beaten path. From the Edwards Lake cabin continue east.");
      /*
      $.ajax({
          url: '/routeAttJSON',   //The server endpoint we are connecting to
          type: 'get',
          data: {
            fileName:'uploads/' + fileName,
            compName: Name,
          },
          success: function (data) {
              var attObj = JSON.parse(data);
              var attributes = "";
              for (var i = 0; i < attObj.length; i++) {
                attributes.append("name of attribute:");
              }
            alert(attributes);
          },
          fail: function(error) {
              console.log(error);
          }
      });*/
    });

    $('#search').click(function(e){
      var startlat = document.getElementById("startLat");
      let stLat = startlat.value;
      var startlon = document.getElementById("startLon");
      let stLon = startlon.value;
      var endlat = document.getElementById("endLat");
      let eLat = endlat.value;
      var endlon = document.getElementById("endLon");
      let eLon = endlon.value;
      if(isNaN(stLat) == false && isNaN(stLon) == false && isNaN(eLat) == false && isNaN(eLon) == false){
        alert("Some route contains a path between points: " + stLat + ", " + stLon + " and: " + eLat + ", " + eLon);
      }
      else{
        alert("longitude and latitude values must be numbers");
      }
    });

    $('#createRt').click(function(e){
      var getName = document.getElementById("routeName");
      let name = getName.value;
      var getwp1 = document.getElementById("wp1");
      let wp1 = getwp1.value;
      var getwp2 = document.getElementById("wp2");
      let wp2 = getwp2.value;
      alert("created route: " + name + " with waypoints: " + wp1 + ", " + wp2);
    });


    $('#createGPXdoc').click(function(e){
      var getName = document.getElementById("enterFileName");
      let name = getName.value;
      var getwp1 = document.getElementById("wp3");
      let wp1 = getwp1.value;
      var getwp2 = document.getElementById("wp4");
      let wp2 = getwp2.value;
      alert("created new document: " + name + " with waypoints: " + wp1 + ", " + wp2);
      $.ajax({
          url: '/writeGPX',
          type: 'get',
          data: {
            json:"{\"version\":1.0,\"creator\":\"Dennis\"}",
            fileName:'upload/' +name+'.gpx',
          },
          success: function (data) {
            //  alert("created new document: " + name + " with waypoints: " + wp1 + ", " + wp2);
          },
          fail: function(error) {
              console.log(error);
          }
        });
    });

    function updateFileList(){
      $.ajax({
          type:'get',
          dataType:'json',
          url:'/getFiles',
          success: function (data) {
              let x = document.getElementById("fileDropDown");
              while (x.length > 0) {
                x.remove(0);
              }
              for (let i = 0; i < data.length; i++) {
                  let option = document.createElement("option");
                  console.log("option");
                  option.text = data[i];
                  x.add(option);
              }
          },
          fail: function(error) {
              console.log(error);
          }
      });
  }

  function updateFileTable(){
    $.ajax({
        type:'get',
        dataType:'json',
        url:'/getFiles',
        success: function (data) {
            for (let i = 0; i < data.length; i++) {
              let fileName = data[i];
              $.ajax({
                  url: '/getJSON',   //The server endpoint we are connecting to
                  type: 'get',            //Request type
                  //dataType: 'json',       //Data type - we will use JSON for almost everything
                  data: {
                      fileName:'uploads/' + fileName,
                  },
                  success: function (data) {
                    console.log(data);
                    let GPXobj = JSON.parse(data);
                    var tableData = "<tr> <td><a href=uploads/"+fileName+">"+fileName+"</a></td> <td>"+GPXobj.version+"</td> <td>"+GPXobj.creator+"</td> <td>"+GPXobj.numWaypoints+"</td> <td>"+GPXobj.numRoutes+"</td> <td>"+GPXobj.numTracks+"</td> </tr>";
                    $('#fileTable').append(tableData);
                    //alert("pressed");
                  },
                  fail: function(error) {
                      // Non-200 return, do something with error
                      //$('#blah').html("On upload, received error from server");
                      console.log(error);
                  }
              });
            }
        },
        fail: function(error) {
            console.log(error);
        }
    });
}

});
