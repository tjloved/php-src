--TEST--
Verifies the correct conversion of objects to arrays
--FILE--
<?php

class foo
{
    private $private = 'private';
    protected $protected = 'protected';
    public $public = 'public';
}
function fn1314444080()
{
    var_export((array) new foo());
}
fn1314444080();
--EXPECTF--
array (
  '' . "\0" . 'foo' . "\0" . 'private' => 'private',
  '' . "\0" . '*' . "\0" . 'protected' => 'protected',
  'public' => 'public',
)
