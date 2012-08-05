<?php
//************************************************************************
// wraithe/src/php/generate.php
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

function ListArticles() {
	global $articles;

	foreach ($articles as $post) {
		echo $post->targetFile . ' ';
	}
}

function OutputTemplate($target) {
	global $articles;

	$post = $articles[$target];
	$template = $post->template;
	//var_dump($post);

	include $post->template;
}
