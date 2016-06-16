--TEST--
Bug #26010 (private / protected variables get exposed by get_object_vars())
--FILE--
<?php

class foo
{
    private $private = 'private';
    protected $protected = 'protected';
    public $public = 'public';
}
function fn2080991163()
{
    $data = new foo();
    $obj_vars = get_object_vars($data);
    var_dump($obj_vars);
}
fn2080991163();
--EXPECT--
array(1) {
  ["public"]=>
  string(6) "public"
}

