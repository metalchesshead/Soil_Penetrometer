# Soil_Penetrometer

Install Python Flask by following the Flask documentation: https://flask.palletsprojects.com/en/stable/

To run the server, execute this command:
flask --app server run --cert=adhoc --host=0.0.0.0

"-cert=adhoc" configures it to be an https server, and "--host=0.0.0.0" makes the server visible to any device on the network, which is necessary since we need a cellphone to connect to it.

Once the server is running, take the Public IP address (not the localhost one) and replace the current value of "String servername" with that one. This is important so that the esp32 can connect to the server. To connect to the phone to this server simply type that address into the url.

When collecting data, first open the webapp on a cellphone, then press "getdata", and wait until the coordinates show up on the screen. Then, open the VSCode serial monitor, and type 'z' to zero the scale, making sure that the tip is not touching anything. Press 's' when ready to record, and press any key to stop.

To plot the data, add "postprocss.py" into any of the folders that were created and run it.
