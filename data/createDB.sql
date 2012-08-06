create table controller (
		last_write_dttm
);

insert into controller values ('2012-01-01 00:00:00');

create table page_types (
		id INTEGER PRIMARY KEY AUTOINCREMENT
	,	type
	,	template_path
	,	output_path
);

insert into page_types (type, template_path, output_path) values ('page', 'data/template/page.php', 'data/public/');
insert into page_types (type, template_path, output_path) values ('post', 'data/template/post.php', 'data/public/post/');

create table articles (
		id INTEGER PRIMARY KEY AUTOINCREMENT
	,	page_type_id
	,	create_dttm
	,	update_dttm
	,	write_dttm
	,	hit_counter
	,	file_name
	,	title
	,	keywords
	,	text_crc
	,	text
);
