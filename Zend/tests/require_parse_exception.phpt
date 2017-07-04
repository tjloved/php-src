--TEST--
Parse exceptions when using require
--INI--
allow_url_include=1
--FILE--
<?php

function test_parse_error($code)
{
    try {
        require 'data://text/plain;base64,' . base64_encode($code);
    } catch (ParseError $e) {
        echo $e->getMessage(), " on line ", $e->getLine(), "\n";
    }
}
function fn17092687()
{
    test_parse_error(<<<'EOC'
<?php
{ { { { { }
EOC
);
    test_parse_error(<<<'EOC'
<?php
/** doc comment */
function f() {
EOC
);
    test_parse_error(<<<'EOC'
<?php
empty
EOC
);
    test_parse_error('<?php
var_dump(078);');
    test_parse_error('<?php
var_dump("\\u{xyz}");');
    test_parse_error('<?php
var_dump("\\u{ffffff}");');
}
fn17092687();
--EXPECTF--
syntax error, unexpected end of file on line %d
syntax error, unexpected end of file on line %d
syntax error, unexpected end of file, expecting '(' on line %d
Invalid numeric literal on line %d
Invalid UTF-8 codepoint escape sequence on line %d
Invalid UTF-8 codepoint escape sequence: Codepoint too large on line %d
