from docx import Document
from docx.shared import Inches, Pt, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.enum.section import WD_SECTION
from docx.enum.table import WD_TABLE_ALIGNMENT, WD_CELL_VERTICAL_ALIGNMENT
from docx.oxml import OxmlElement
from docx.oxml.ns import qn
from docx.oxml.shared import OxmlElement as SharedOxmlElement
from pathlib import Path

# Source path
src = Path('/mnt/data/Pasted text(6).txt')
text = src.read_text(encoding='utf-8')

doc = Document()

# Page setup
section = doc.sections[0]
section.top_margin = Inches(0.7)
section.bottom_margin = Inches(0.7)
section.left_margin = Inches(0.7)
section.right_margin = Inches(0.7)

# Default font
styles = doc.styles
styles['Normal'].font.name = 'Calibri'
styles['Normal']._element.rPr.rFonts.set(qn('w:eastAsia'), 'Calibri')
styles['Normal'].font.size = Pt(11)

def set_cell_shading(cell, fill):
tcPr = cell._tc.get_or_add_tcPr()
shd = OxmlElement('w:shd')
shd.set(qn('w:fill'), fill)
tcPr.append(shd)

def set_cell_margins(cell, top=60, start=60, bottom=60, end=60):
tc = cell._tc
tcPr = tc.get_or_add_tcPr()
tcMar = tcPr.first_child_found_in("w:tcMar")
if tcMar is None:
tcMar = OxmlElement('w:tcMar')
tcPr.append(tcMar)
for m, v in [('top', top), ('start', start), ('bottom', bottom), ('end', end)]:
node = tcMar.find(qn(f'w:{m}'))
if node is None:
node = OxmlElement(f'w:{m}')
tcMar.append(node)
node.set(qn('w:w'), str(v))
node.set(qn('w:type'), 'dxa')

def add_hyperlink(paragraph, text, url, color="0563C1", underline=True):
part = paragraph.part
r_id = part.relate_to(url, "http://schemas.openxmlformats.org/officeDocument/2006/relationships/hyperlink", is_external=True)
hyperlink = OxmlElement('w:hyperlink')
hyperlink.set(qn('r:id'), r_id)

new_run = OxmlElement('w:r')
rPr = OxmlElement('w:rPr')
if color:
c = OxmlElement('w:color')
c.set(qn('w:val'), color)
rPr.append(c)
if underline:
u = OxmlElement('w:u')
u.set(qn('w:val'), 'single')
rPr.append(u)
new_run.append(rPr)
t = OxmlElement('w:t')
t.text = text
new_run.append(t)
hyperlink.append(new_run)
paragraph._p.append(hyperlink)
return hyperlink

def add_title(text):
p = doc.add_paragraph()
p.alignment = WD_ALIGN_PARAGRAPH.CENTER
run = p.add_run(text)
run.font.name = 'Calibri'
run._element.rPr.rFonts.set(qn('w:eastAsia'), 'Calibri')
run.font.size = Pt(20)
run.bold = True
run.underline = True
return p

def add_center_heading(text):
return add_title(text)

def add_subheading(text, size=14):
p = doc.add_paragraph()
p.alignment = WD_ALIGN_PARAGRAPH.LEFT
r = p.add_run(text)
r.bold = True
r.font.size = Pt(size)
r.font.name = 'Calibri'
r._element.rPr.rFonts.set(qn('w:eastAsia'), 'Calibri')
return p

def add_para(text, italic=False, bold=False, size=11):
p = doc.add_paragraph()
p.alignment = WD_ALIGN_PARAGRAPH.LEFT
r = p.add_run(text)
r.italic = italic
r.bold = bold
r.font.size = Pt(size)
r.font.name = 'Calibri'
r._element.rPr.rFonts.set(qn('w:eastAsia'), 'Calibri')
return p

def add_code_block(code_lines):
# code_lines can be list or string
if isinstance(code_lines, str):
code_lines = code_lines.splitlines()
for line in code_lines:
p = doc.add_paragraph()
p.paragraph_format.left_indent = Inches(0.25)
p.paragraph_format.space_before = Pt(0)
p.paragraph_format.space_after = Pt(0)
# light shading
pPr = p._p.get_or_add_pPr()
shd = OxmlElement('w:shd')
shd.set(qn('w:fill'), 'F3F3F3')
pPr.append(shd)
r = p.add_run(line)
r.font.name = 'Courier New'
r._element.rPr.rFonts.set(qn('w:eastAsia'), 'Courier New')
r.font.size = Pt(9)
doc.add_paragraph() # blank after block

def add_bullets(items, level=0):
for item in items:
p = doc.add_paragraph(style='List Bullet')
if level:
p.paragraph_format.left_indent = Inches(0.25 * level)
r = p.add_run(item)
r.font.size = Pt(11)

def add_numbered(items, level=0):
for item in items:
p = doc.add_paragraph(style='List Number')
if level:
p.paragraph_format.left_indent = Inches(0.25 * level)
r = p.add_run(item)
r.font.size = Pt(11)

def add_table(headers, rows, col_widths=None, font_size=9, header_fill='D9EAF7'):
table = doc.add_table(rows=1, cols=len(headers))
table.style = 'Table Grid'
table.alignment = WD_TABLE_ALIGNMENT.CENTER
table.autofit = False
hdr = table.rows[0].cells
for i, h in enumerate(headers):
hdr[i].text = str(h)
hdr[i].vertical_alignment = WD_CELL_VERTICAL_ALIGNMENT.CENTER
for p in hdr[i].paragraphs:
p.alignment = WD_ALIGN_PARAGRAPH.CENTER
for run in p.runs:
run.bold = True
run.font.size = Pt(font_size)
run.font.name = 'Calibri'
run._element.rPr.rFonts.set(qn('w:eastAsia'), 'Calibri')
set_cell_shading(hdr[i], header_fill)
set_cell_margins(hdr[i])
for row in rows:
cells = table.add_row().cells
for i, val in enumerate(row):
if isinstance(val, tuple) and val[0] == 'hyperlink':
# tuple format: ('hyperlink', display, url)
p = cells[i].paragraphs[0]
add_hyperlink(p, val[1], val[2])
else:
cells[i].text = '' if val is None else str(val)
cells[i].vertical_alignment = WD_CELL_VERTICAL_ALIGNMENT.CENTER
for p in cells[i].paragraphs:
if p.text:
p.alignment = WD_ALIGN_PARAGRAPH.LEFT if i != 0 else WD_ALIGN_PARAGRAPH.CENTER
for run in p.runs:
run.font.size = Pt(font_size)
run.font.name = 'Calibri'
run._element.rPr.rFonts.set(qn('w:eastAsia'), 'Calibri')
set_cell_margins(cells[i])
if col_widths:
for row in table.rows:
for idx, width in enumerate(col_widths):
row.cells[idx].width = width
return table

# Title block
add_title("Pusher 1.0")
add_para("June 8, 2026", italic=True, size=11)
add_para("Affiliation: Fallout, Hackclub", size=11)
doc.add_paragraph()

# Intro paragraph
intro = (
"Pusher is a 4 Degree of Freedom wearable robotic arm based on the anatomical structure of the human arm, "
"with a maximum reach of ~0.65 meters and a weight of 5 kilograms. It is designed for modularity, aiming "
"to serve as an affordable, general-purpose research platform for Human-Computer Interfacing, such as studies "
"involving EMG, EEG, and Computer Vision, to assist in the control of supernumerary arms. "
"This project is intended for undergraduate researchers and hobbyists looking for an introduction to HCI robotics, "
"with potential for graduate-school prototyping because it is easy to produce and low cost."
)
add_para(intro)
doc.add_paragraph()

# Table of contents
add_center_heading("Table of Contents")
toc_items = [
"Bill of Materials",
"System Diagram",
"Building the Project",
"Prerequisites",
"Assembling Mechanical Design",
"Assembling Electrical Design",
"Flashing Firmware",
"Author's Notes",
"Commentary on BOM",
"Guide to Adding Implementations (Taking Advantage of Modularity)",
"Future Implementations by Author",
]
add_numbered(toc_items)
doc.add_paragraph()

# Bill of Materials
add_center_heading("Bill of Materials")

# Electronics BOM table
add_subheading("Electronics", size=14)
electronics_headers = ["Index", "LCSC #", "Quantity", "Value", "Link"]
electronics_rows = [
["1", "C146170", "2", "1.81", ("hyperlink", "LCSC", "https://www.lcsc.com/product-detail/C146170.html")],
["2", "C22396880", "6", "10.34", ("hyperlink", "LCSC", "https://www.lcsc.com/product-detail/C22396880.html")],
["3", "C49678", "100", "0.84", ("hyperlink", "LCSC", "https://www.lcsc.com/product-detail/C49678.html")],
["4", "C72487", "20", "0.57", ("hyperlink", "LCSC", "https://www.lcsc.com/product-detail/C72487.html")],
["5", "C24123", "20", "0.30", ("hyperlink", "LCSC", "https://www.lcsc.com/product-detail/C24123.html")],
["6", "C3869051", "20", "0.57", ("hyperlink", "LCSC", "https://www.lcsc.com/product-detail/C3869051.html")],
["7", "C8465", "5", "0.67", ("hyperlink", "LCSC", "https://www.lcsc.com/product-detail/C8465.html")],
["8", "C72334", "5", "1.01", ("hyperlink", "LCSC", "https://www.lcsc.com/product-detail/C72334.html")],
["9", "C86513", "4", "6.79", ("hyperlink", "LCSC", "https://www.lcsc.com/product-detail/C86513.html")],
["10", "C114548", "100", "0.36", ("hyperlink", "LCSC", "https://www.lcsc.com/product-detail/C114548.html")],
["11", "C2930231", "100", "0.19", ("hyperlink", "LCSC", "https://www.lcsc.com/product-detail/C2930231.html")],
["12", "C90964", "5", "3.70", ("hyperlink", "LCSC", "https://www.lcsc.com/product-detail/C90964.html")],
["13", "C58159", "1", "0.38", ("hyperlink", "LCSC", "https://www.lcsc.com/product-detail/C58159.html")],
["14", "C7467397", "50", "0.65", ("hyperlink", "LCSC", "https://www.lcsc.com/product-detail/C7467397.html")],
["15", "C2930266", "100", "0.19", ("hyperlink", "LCSC", "https://www.lcsc.com/product-detail/C2930266.html")],
["16", "C2907315", "100", "0.22", ("hyperlink", "LCSC", "https://www.lcsc.com/product-detail/C2907315.html")],
["", "", "Total:", "28.59", ""],
]
add_table(electronics_headers, electronics_rows, font_size=8)

doc.add_paragraph()

# Mechanical BOM table
add_subheading("Mechanical", size=14)
mechanical_headers = ["Part name", "Quantity", "Value", "Link"]
mechanical_rows = [
["PLA Filament", "1", "7.36", ("hyperlink", "AliExpress", "https://www.aliexpress.us/item/3256806054300015.html?spm=a2g0o.productlist.main.3.6f78379d5OwR1X&algo_pvid=498c2f52-b8c2-4add-bea6-f9bda40c924e&algo_exp_id=498c2f52-b8c2-4add-bea6-f9bda40c924e-2&pdp_ext_f=%7B%22order%22%3A%22488%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21USD%2122.79%217.36%21%21%2122.79%217.36%21%402140c1c317807875296987000e9684%2112000058209259440%21sea%21US%210%21ABX%211%210%21n_tag%3A-29910%3Bd%3Ae1bbef76%3Bm03_new_user%3A-29895%3BpisId%3A5000000208023469&curPageLogUid=Qx4M7j8exyPx&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005006240614767%7C_p_origin_prod%3A")],
["motors", "4", "30", ("hyperlink", "Precision Mini Drives", "https://precisionminidrives.com/product/shaft-diameter-8mm-10mm-micro-dc-gear-motor-model-nfp-42-775-ce-12v-24v?attribute_option=12V%2C+80RPM&attribute_shaft-diameter=10mm&utm_source=GoogleAds&utm_campaign=PMax&utm_medium=Google%20Shopping&utm_term=adtribes&utm_content=%7Bproduct_id%7D&gad_source=1&gad_campaignid=18478080173&gbraid=0AAAAAomY7RXK-6ddvY5v_P-wvX2-wvXLt&gclid=CjwKCAjw5NvPBhAoEiwA_2egfvITADIqACLw4gyQt7JJ5pv2qg0MlFNwTrXCpuVo1aJEWzca6XUyrBoCjcwQAvD_BwE")],
["RU-28", "4", "19.91", ("hyperlink", "AliExpress", "https://www.aliexpress.us/item/3256809392312866.html?src=google&src=google&albch=shopping&acnt=708-803-3821&isdl=y&slnk=&plac=&mtctp=&albbt=Google_7_shopping&aff_platform=google&aff_short_key=UneMJZVf&gclsrc=aw.ds&albagn=888888&ds_e_adid=&ds_e_matchtype=&ds_e_device=c&ds_e_network=x&ds_e_product_group_id=&ds_e_product_id=en3256809392312866&ds_e_product_merchant_id=107675220&ds_e_product_country=US&ds_e_product_language=en&ds_e_product_channel=online&ds_e_product_store_id=&ds_url_v=2&albcp=20123152476&albag=&isSmbAutoCall=false&needSmbHouyi=false&gad_source=1&gad_campaignid=20127768206&gbraid=0AAAAAD6I-hH5MzbM-Qht5WI26YJI1jQ7x&gclid=CjwKCAjw5NvPBhAoEiwA_2egfknGl0bGc7tc0ydw4pmODKzmqiRzPcj2s5EQ8m0HpaPSL7tY9xceahoCLmEQAvD_BwE&gatewayAdapt=glo2usa")],
["M5 Screws (x50)", "1", "15", ("hyperlink", "Amazon Japan", "https://www.amazon.co.jp/-/en/Button-Socket-Screws-Finish-BHSSM5X10B-50-SL/dp/B07DZN3FP5")],
["M3 screws and nuts (x20)", "1", "9", ("hyperlink", "Amazon Japan", "https://www.amazon.co.jp/-/en/Phillips-Machine-Washers-Plastic-Countersunk/dp/B0B9YGNKV6")],
["m5 nuts (x50)", "1", "13", ("hyperlink", "Amazon Japan", "https://www.amazon.co.jp/-/en/Thin-Nuts-Stainless-Steel-Flat/dp/B0CQ55XWQX?th=1")],
["", "", "Total:", "244", ""],
]
add_table(mechanical_headers, mechanical_rows, font_size=8)
add_para("Total project cost: 272.59", bold=True)
doc.add_paragraph()

# System diagram
add_center_heading("System Diagram")
add_para("[Image omitted from source document.]")
doc.add_paragraph()

# Building project sections
add_center_heading("Building the Project")
add_subheading("Prerequisites", size=14)
add_para("This source document does not provide a completed prerequisites list. The following sections cover mechanical assembly, electrical assembly, and flashing steps.")
doc.add_paragraph()

add_center_heading("Assembling Mechanical Design")
add_para("For assembly, please see the [FINAL ASSEMBLY] file in the mechanical design folder, opened in CAD software such as Fusion 360.")
add_para("The material modeled for stress testing and manufacturability is PLA plastic, intended for 3D printers such as Bambu models.")
add_para("In a slicer such as Cura, print the following:")
add_bullets([
"1x Bicep_And_Cap",
"1x Bicep_RU_28_Connector (in Bicep_And_Cap)",
"1x Forearm",
"3x Joint_Cover",
"1x Joint1",
"2x Joints_2_and_3",
"3x RU_28_Connector",
])
add_para("Approximately ~2 kg in PLA.")
add_para("To assemble the physical design, it is recommended to have 30x M5 screws (10 mm) and 30x matching nuts. Then, use 15x M3 screws (10 mm) with 15x matching nuts.")
doc.add_paragraph()

add_center_heading("Assembling Electrical Design")
p = doc.add_paragraph()
p.add_run("Trusted vendors: ").bold = True
add_hyperlink(p, "JLCPCB", "https://jlcpcb.com/")
p.add_run(" and ")
add_hyperlink(p, "LCSC", "https://www.lcsc.com/")
p.add_run(".")
add_para("After ordering your PCB and components from JLCPCB and LCSC, hand-solder them as follows:")
add_bullets([
"4x motor driver PCBs",
"1x main controller PCB",
])
add_para("Motor driver PCBs are secured in a dedicated mount, which is fastened to the joints themselves.")
add_para("The controller PCB is left separate from the rest of the arm. The recommended option is to keep it on a desk during assembly and strap it to the arm during operation.")
add_subheading("Wiring diagram", size=12)
add_para("Red: 24 V and 3.3 V power supply bus")
add_para("Yellow: I2C communication bus")
add_para("Connections:")
add_bullets([
"From Joint 1 to Joint 2",
"From Joint 2 to Joint 3",
"From Joint 3 to Bicep",
])
add_para("The firmware includes a 360-degree clamp to prevent wires from excessive twisting and strain.")
doc.add_paragraph()

add_center_heading("Flashing Firmware")
add_center_heading("Flashing STM32F103C8T6")
add_para("Note: The firmware should be placed in the Brushed_DC_Motor_Controller_STM32 folder. This is the folder you select when creating a project from an existing STM32CubeIDE project. The pinouts and clock configuration are omitted from the source text, so only the text instructions are preserved here.")
add_para("Step 1: Connect SWDIO, SWCLK, GND, and 3V3 on the motor driver STM32 to the corresponding pins on your ST-LINK. A STM32 Nucleo board can be used to avoid third-party flashers that do not work.")
add_para("Remove the ST-Link isolation jumpers on CN2 of the Nucleo board so that you program the DC motor driver rather than the Nucleo itself.")
add_para("Step 2: Import Brushed_DC_Motor_Controller_STM32 as an Existing STM32 project into STM32CubeIDE.")
add_para("Step 3: Open the project, find the build tool on the top toolbar, and click it. Verify that there are no errors. If workspace configuration causes errors, close STM32CubeIDE, reopen it, and select the Software folder as the workspace.")
add_para("Step 4: Click the debug tool.")
add_para("Step 5: Click Run.")
add_para("If flashing succeeds, STM32CubeIDE should display a successful download message in the console.")
doc.add_paragraph()

add_center_heading("Flashing ESP32")
add_para("Step 1: Follow the steps in the AKP's IoT Tech video to download and set up ESP32 IDF. Stop around 4:20, after creating a project folder.")
p = doc.add_paragraph()
add_hyperlink(p, "YouTube video", "https://www.youtube.com/watch?v=xQ-Zk0FaK-c")
add_para("Step 2: At 4:20, replace the code in hello_world_main.c with the contents of main.c located in the ESP32 Joystick Controller folder.")
add_para("The code does not use external libraries, so no additional imports are needed.")
add_para("Step 3:")
add_para("In the ESP32-IDF terminal, type:")
add_code_block(["idf.py set-target esp32", "idf.py build"])
add_para("Step 4: Connect the ESP32 to your computer via USB.")
add_para("On Windows, select the ESP32 model and the communication port. Ports usually appear as COMx.")
add_para("Then run:")
add_code_block(["idf.py -p COM3 flash"])
add_para("Replace 3 in COM3 with the corresponding COM number.")
add_subheading("Mac", size=12)
add_para("On Mac, ports usually appear as /dev/ttyUSBx.")
add_para("You can identify the port with:")
add_code_block(['ls -la /dev | grep -E "cu\\.(SLAB|usbserial)"'])
add_para("Then follow these steps:")
add_code_block([
"source ~/esp/esp-idf/export.sh",
"idf.py set-target esp32",
"idf.py -p /dev/cu.SLAB_USBtoUART flash",
])
add_subheading("Linux", size=12)
add_para("On Linux, ports usually appear as /dev/ttyACMx.")
add_para("First, add your user to the hardware access group:")
add_para("Ubuntu/Debian: sudo usermod -aG dialout $USER")
add_para("Arch Linux: sudo usermod -aG uucp $USER")
add_para("Then, find the device path with:")
add_code_block(["ls /dev/ttyUSB* /dev/ttyACM*"])
add_para("Then navigate into your project directory and select the model:")
add_code_block(["idf.py set-target esp32"])
add_para("Finally, flash and monitor:")
add_code_block(["idf.py -p /dev/ttyUSB0 flash monitor"])
add_para("Replace /dev/ttyUSB0 with the actual device path identified above.")

# Save
out = Path('/mnt/data/Pusher_1_0_cleaned.docx')
doc.save(out)
print(out)
