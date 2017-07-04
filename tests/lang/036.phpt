--TEST--
Child public element should not override parent private element in parent methods
--FILE--
<?php

class par
{
    private $id = "foo";
    function displayMe()
    {
        print $this->id;
    }
}
class chld extends par
{
    public $id = "bar";
    function displayHim()
    {
        parent::displayMe();
    }
}
function fn404887210()
{
    $obj = new chld();
    $obj->displayHim();
}
fn404887210();
--EXPECT--
foo
