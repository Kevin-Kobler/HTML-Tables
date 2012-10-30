The goal of this project is to create an interactive html table that can be
modified and updated by the owner with ease.

The structure of this project will mainly be C/CGI/UNIX, some JavaScript will
be used in order to create a more user friendly interface.

	-UNIX will be the backbone of the system, holding the file that
	 contains all the information for the table.

	-C code will process information from a table, modifing the data file
	 containg that information and creating an HTML table.

	-CGI will connect the webrequest and C code.

TODO:
	
	Create a program, table_to_HTML, that converts data from -- csv, tsv or
	any sort of delimited text file -- to an HMTL Table.
	
	Create a program, update_table, that takes input from a user and
	modifies the table file accordingly

	Create a CGI script to allow modification from a website	

	Write JavaScript to make the website spiffy.

EXTRAS:
	
	Create a program, HTML_to_table, that takes data from an HTML page
        and creates a tsv file.
