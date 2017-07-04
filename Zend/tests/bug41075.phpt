--TEST--
Bug #41075 (memleak when creating default object caused exception)
--FILE--
<?php

function err($errno, $errstr, $errfile, $errline)
{
    throw new Exception($errstr);
}
class test
{
    function foo()
    {
        $var = $this->blah->prop = "string";
        var_dump($this->blah);
    }
}
function fn1924451049()
{
    set_error_handler("err");
    $t = new test();
    try {
        $t->foo();
    } catch (Exception $e) {
        var_dump($e->getMessage());
    }
    echo "Done\n";
}
fn1924451049();
--EXPECTF--	
string(40) "Creating default object from empty value"
Done
