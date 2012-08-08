<?php
//************************************************************************
// wraithe/src/php/makePost.php
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

// override paths if needed

global $model;

//************************************************************************
//
class WraitheLink {
	private $name;
	private $url;

	public function __construct($name, $url) {
		$this->name = $name;
		$this->url  = $url;
	}

	public function Name() {
		return $this->name;
	}
	public function URL() {
		return $this->url;
	}
}

//************************************************************************
// define the view
class WraitheView {
	private $dbh;
	private $rootPath;
	private $templateFile;
	private $outputFile;
	private $article;
	private $articleID;
	private $model;

	public function __construct($dbFileName) {
		print "hey, opening {$dbFileName}\n";
		try {
			$this->dbh = new PDO("sqlite:{$dbFileName}");
			$this->dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
		} catch (PDOException $e) {
			print "error: {$e}\n";
			die($e);
		}
	}

	public function GetArticle($id) {
		$this->article   = null;
		$this->articleID = $id;
		$this->model     = null;

		$sql   = "select template_path";
		$sql  .= "     , create_dttm";
		$sql  .= "     , update_dttm";
		$sql  .= "     , file_name";
		$sql  .= "     , keywords";
		$sql  .= "     , title";
		$sql  .= "     , text";
		$sql  .= "  from articles";
		$sql  .= "     , page_types";
		$sql  .= " where articles.id   = :id";
		$sql  .= "   and page_types.id = articles.page_type_id";

		$stmt = $this->dbh->prepare($sql);
		$stmt->setFetchMode(PDO::FETCH_OBJ);  
		$stmt->bindParam(':id', $id);
		$stmt->execute();
		while ($row = $stmt->fetch()) {
			$this->templateFile = $row->template_path;
			$this->outputFile   = $row->file_name;
			$this->article = new WraitheArticle($row->title, $row->create_dttm, $row->update_dttm, $row->author, $row->keywords, $row->text);
		}

		return $this->article;
	}

	public function GetModel($id) {
		$this->model = new WraitheModel($this, $this->GetArticle($id));
		return $this->model;
	}

	public function GetPopularArticles() {
		$sql    = 'select title, url from articles order by hit_counter desc, id asc limit 4';
		$idx    = 0;
		$result = array();

		foreach($this->dbh->query($sql) as $row) {
			$result[$idx] = new WraitheLink($row['title'], $row['url']);
			$idx = $idx + 1;
		}

		return $result;
	}
	public function GetRecentArticles() {
		$sql    = 'select title, url from articles order by update_dttm desc, id desc limit 4';
		$idx    = 0;
		$result = array();

		foreach($this->dbh->query($sql) as $row) {
			$result[$idx] = new WraitheLink($row['title'], $row['url']);
			$idx = $idx + 1;
		}

		return $result;
	}

	public function OutputFile() {
		return $this->outputFile;
	}

	// return array of article ID
	//
	public function OutOfDateArticles() {
		$sql    = 'select id from articles where write_dttm is null order by id';
		$idx    = 0;
		$result = array();

		foreach($this->dbh->query($sql) as $row) {
			$result[$idx] = $row;
			$idx = $idx + 1;
		}

		return $result;
	}

	public function RootPath() {
		return $this->rootPath;
	}

	public function TemplateFile() {
		return $this->templateFile;
	}

	public function UpdateArticleWriteDttm($id) {
		$sql  = "update articles";
		$sql .= "   set write_dttm = datetime()";
		$sql .= " where id         = :id";

		$stmt = $this->dbh->prepare($sql);
		$count = $stmt->execute(array(':id' => $id));
		if ($count > 0) {
			return true;
		}
		return false;
	}
}

//************************************************************************
// define an article
class WraitheArticle {
	private $author;
	private $createDttm;
	private $keywords;
	private $text;
	private $title;
	private $updateDttm;

	public function __construct($title, $create_dttm, $update_dttm, $author, $keywords, $text) {
		$this->author     = $author;
		$this->createDttm = $createDttm;
		$this->keywords   = $keywords;
		$this->text       = $text;
		$this->title      = $title;
		$this->updateDttm = $updateDttm;
	}

	public function Author() {
		return $this->author;
	}
	public function CreateDttm() {
		return $this->createDttm;
	}
	public function FileName() {
		return $this->fileName;
	}
	public function HitCounter() {
		return $this->hitCounter;
	}
	public function Keywords() {
		return $this->keywords;
	}
	public function PageType() {
		return $this->pageType;
	}
	public function Text() {
		return $this->text;
	}
	public function Title() {
		return $this->title;
	}
	public function UpdateDttm() {
		return $this->updateDttm;
	}
}

//************************************************************************
// define the model
class WraitheModel {
	private $view;
	private $article;
	private $disqusID;

	public function __construct(WraitheView $view, WraitheArticle $article) {
		$this->view     = $view;
		$this->article  = $article;
		$this->disqusID = $view->OutputFile();
	}

	public function Author() {
		return $this->article->Author();
	}
	public function Content() {
		return $this->article->Text();
	}
	public function CopyrightYear() {
		return "2012";
	}
	public function CreateDttm() {
		return $this->article->CreateDttm();
	}
	public function DisqusID() {
		return $this->disqusID;
	}
	public function Description() {
		return "produced by wraithe";
	}
	public function FooterDttmText() {
		if ($this->article->CreateDttm() == $this->article->UpdateDttm()) {
			return "Created on " . $this->createDttm;
		}
		return "Updated on " . $this->updateDttm;
	}
	public function Keywords() {
		return $this->article->Keywords();
	}
	public function PopularArticles() {
		return $this->view->GetPopularArticles();
	}
	public function RecentArticles() {
		return $this->view->GetRecentArticles();
	}
	public function Title() {
		return $this->article->Title();
	}
	public function UpdateDttm() {
		return $this->article->UpdateDttm();
	}
}

//************************************************************************
// define the controller
class WraitheController {
	private $rootPath;
	private $templatePath;
	private $fileName;

	public function __construct($rootPath, $templatePath, $fileName) {
		$this->rootPath     = $rootPath;
		$this->templatePath = $templatePath;
		$this->fileName     = $fileName;
	}

	// render an article
	public function Render($id, WraitheModel $model_) {
		print "rendr:\tarticle_id {$id} {$this->templatePath}\n";

		// set up the global variable model that the templates will use
		$model = $model_;

    ob_start();
    require $this->templatePath;
    $text = ob_get_contents();
    ob_end_clean();

    return $text;
	}
}

//************************************************************************
// create the view
$wrv = new WraitheView('data/post.mdb');

//************************************************************************
// then create a controller for each article in the view. render the
// article, then try to save it to the file
//
foreach ($wrv->OutOfDateArticles() as $row) {
	$id    = $row['id'];
	$model = $wrv->GetModel($id);
	$wrc   = new WraitheController($wrv->RootPath(), $wrv->TemplateFile(), $wrv->OutputFile());

	$text = $wrc->Render($row['id'], $model);

	$disqusID = $wrv->OutputFile();

	$handle = fopen($wrv->OutputFile(), 'w') or die("error:\tcan not create {$wrv->OutputFile()}");
	fwrite($handle, $text);
	fclose($handle);

	$wrv->UpdateArticleWriteDttm($id);

	$wrc = null;
}

// load the database

// for each type of page
  // for each article that is out of date
    // render the template
    // write the results to the file
