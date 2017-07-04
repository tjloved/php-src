--TEST--
Bug #30889 (Conflict between __get/__set and ++ operator)
--FILE--
<?php

class overloaded
{
    private $values;
    function __construct()
    {
        $this->values = array('a' => 0);
    }
    function __set($name, $value)
    {
        print "set {$name} = {$value} ({$name} was " . $this->values[$name] . ")\n";
        $this->values[$name] = $value;
    }
    function __get($name)
    {
        print "get {$name} (returns " . $this->values[$name] . ")\n";
        return $this->values[$name];
    }
}
function fn1558720383()
{
    $test = new overloaded();
    $test->a++;
    // __get(), then __set()
    ++$test->a;
}
fn1558720383();
--EXPECT--
get a (returns 0)
set a = 1 (a was 0)
get a (returns 1)
set a = 2 (a was 1)
