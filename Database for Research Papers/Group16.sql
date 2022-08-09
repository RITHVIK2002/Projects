create table paper(
paper_id int primary key,
abstract varchar(10000),
year int,
title varchar(1000),
venue varchar(1000),
main_author varchar(1000));

copy paper from '/home/rithvik/Downloads/Paper.csv' delimiter ',' csv header;


create table cited_by(
paper_id int references paper(paper_id),
cited_bypaperid int references paper(paper_id)
);


copy cited_by from '/home/rithvik/Downloads/cited_by.csv' delimiter ',' csv header; 


create table listof_co(
paper_id int references paper(paper_id),
Name varchar(1000));


copy listof_co from '/home/rithvik/Downloads/listof_co.csv' delimiter ',' csv header; 
