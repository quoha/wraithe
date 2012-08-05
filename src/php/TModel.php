<?php
//************************************************************************
// wraithe/src/php/TModel.php
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

class TModel {
	public $name;
	public $author;
	public $title;
	public $createDate;
	public $description;
	public $keywords;
	public $sourceFile;
	public $targetFile;
	public $targetURL;
	public $template;
	public $disqusID;

	function __construct($name_, $title_, $createDate_, $description_, $keywords_) {
		global $articles;

		$this->template = 'template/post.tpl';
		$this->name = $name_;
		$this->author = 'Michael D Henderson';
		$this->title = $title_;
		$this->description = $description_;
		$this->keywords = $keywords_;
		$this->sourceFile = 'post/' . $this->name . '.post';
		if ($createDate_ == 'n/a') {
			$this->createDate = '';
			$this->createYear = '';
		} else {
			$this->createDate = $createDate_;
			$this->createYear = substr($createDate_, 0, 4);
		}
		$this->targetURL  = '/post/' . $this->name . '.html';
		$this->targetFile = 'public' . $this->targetURL;
		$this->disqusID   = $this->targetURL;

		$articles[$this->targetFile] = $this;

		$articles['public/index.html'] = clone $this;
		$articles['public/index.html']->targetFile = 'public/index.html';
	}
}
