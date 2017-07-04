--TEST--
ZE2 Data corruption in __set
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
function fn1338914811()
{
    $f = 'c="foo"';
    $f = new foo();
    $f->bar();
}
fn1338914811();
--EXPECT--
__setf
