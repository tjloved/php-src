--TEST--
eval() parse error on function with doc comment
--FILE--
<?php

try {
    eval(<<<EOC
/** doc comment */
function f() {
EOC
    );
} catch (ParseException $e) {
    echo $e;
}

?>
--EXPECTF--
exception 'ParseException' with message 'syntax error, unexpected end of file' in %s(%d) : eval()'d code:2
Stack trace:
#0 {main}
