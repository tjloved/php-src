--TEST--
Testing exception properties
--FILE--
<?php

class my_file
{
    public function __toString()
    {
        return "somebuildfilename";
    }
}
class my_exception extends exception
{
    public function __construct()
    {
        $this->message = new stdclass();
        $this->file = new my_file();
        $this->line = "12";
    }
}
function fn1342012966()
{
    throw new my_exception();
}
fn1342012966();
--EXPECTF--
Catchable fatal error: Object of class stdClass could not be converted to string in Unknown on line %d
