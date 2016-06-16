--TEST--
'Static' binding for private variables
--FILE--
<?php

class par
{
    private $id = "foo";
    function displayMe()
    {
        $this->displayChild();
    }
}
class chld extends par
{
    private $id = "bar";
    function displayChild()
    {
        print $this->id;
    }
}
function fn562020150()
{
    $obj = new chld();
    $obj->displayMe();
}
fn562020150();
--EXPECT--
bar
