.section .rodata
.global my_font
.type my_font, @object
.align 4

my_font:
    .incbin "../external/IosevkaTermNerdFont-Medium.ttf"
    .global my_font_length
    .type my_font_length, @object
    .align 4
my_font_length:
    .int . - my_font
