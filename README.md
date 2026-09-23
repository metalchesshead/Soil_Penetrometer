# Soil_Penetrometer

Install Python Flask by following the Flask documentation: https://flask.palletsprojects.com/en/stable/

To run the server, execute this command:
flask --app server run --cert=adhoc --host=0.0.0.0

"-cert=adhoc" configures it to be an https server, and "--host=0.0.0.0" makes the server visible to any device on the network, which is necessary since we need a cellphone to connect to it.

Once the server is running, take the Public IP address (not the localhost one) and replace the current value of "String servername" with that one. This is important so that the esp32 can connect to the server. To connect to the phone to this server simply type that address into the url.
https://cdn.sparkfun.com/assets/6/a/5/9/d/Qwiic_Scale.pdf
