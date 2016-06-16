--TEST--
Testing 'static::' and 'parent::' in calls
--FILE--
<?php

class bar
{
    public function __call($a, $b)
    {
        print "hello\n";
    }
}
class foo extends bar
{
    public function __construct()
    {
        static::bar();
        parent::bar();
    }
}
function fn524411304()
{
    new foo();
}
fn524411304();
--EXPECT--
hello
hello
