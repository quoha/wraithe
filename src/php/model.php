<?php
//************************************************************************
// wraithe/src/php/model.php
//
// Copyright (c) 2012 Michael D Henderson, all rights reserved
//
// This file is part of wraithe (http://github.com/quoha/wraithe).
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//************************************************************************

$instance = new TModel('bibliography', 'Bibliography', '2012/07/07', 'n/a', 'n/a');

$instance = new TModel('introduction', 'Introduction', '2012/07/07', 'n/a', 'n/a');

$instance = new TModel('our-first-language', 'Our First Language', '2012/07/07', 'n/a', 'n/a');

//var_dump($articles);

$popularArticles = array (
	1 => $articles['public/post/our-first-language.html'],
	2 => $articles['public/post/introduction.html'],
);
