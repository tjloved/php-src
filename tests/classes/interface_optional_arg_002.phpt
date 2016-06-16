--TEST--
default argument value in interface implementation
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

interface test
{
    public function bar();
}
class foo implements test
{
    public function bar($arg = 2)
    {
        var_dump($arg);
    }
}
function fn1470913148()
{
    $foo = new foo();
    $foo->bar();
}
fn1470913148();
--EXPECT--
int(2)
