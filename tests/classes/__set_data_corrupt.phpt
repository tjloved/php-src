--TEST--
ZE2 Data corruption in __set
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 is needed'); ?>
--FILE--
<?php

class foo
{
    const foobar = 1;
    public $pp = array('t' => null);
    function bar()
    {
        echo $this->t = 'f';
    }
    function __get($prop)
    {
        return $this->pp[$prop];
    }
    function __set($prop, $val)
    {
        echo "__set";
        $this->pp[$prop] = '';
    }
}
function fn718977237()
{
    $f = 'c="foo"';
    $f = new foo();
    $f->bar();
}
fn718977237();
--EXPECT--
__setf
