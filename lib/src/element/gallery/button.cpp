/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/gallery/button.hpp>
#include <utility>

namespace cycfi { namespace elements
{
   void draw_button_base(
      context const& ctx, rect bounds, color color_, bool enabled, float corner_radius)
   {
      draw_button(ctx.canvas, bounds, color_, enabled, corner_radius);
   }

   bool button_renderer_base::cursor(context const& ctx, point /*p*/, cursor_tracking /*status*/)
   {
      ctx.view.refresh(ctx);
      return true;
   }

   bool button_renderer_base::wants_control() const
   {
      return true;
   }

   view_limits button_renderer::limits(basic_context const& ctx) const
   {
      auto const& theme = get_theme();
      auto margin = theme.button_margin;
      auto font = theme.label_font;

      // Measure the text width
      auto size = measure_text(ctx.canvas, _label.c_str(), font.size(font._size * _size));

      // Add space for the icon if necessary
      if (_icon_placement != icon_none)
         size.x += (_size * theme.icon_font._size) + 8;

      auto x_space = margin.left + margin.right;
      auto y_space = margin.top + margin.bottom;
      return {
         { size.x + x_space, size.y + y_space }
       , { full_extent, size.y + y_space }
      };
   }

   void button_renderer::draw(context const& ctx)
   {
      auto& cnv = ctx.canvas;
      auto canvas_state = cnv.new_state();
      auto const& theme = get_theme();
      auto bounds = ctx.bounds;
      auto margin = theme.button_margin;

      auto state = value();
      auto value = state.value;
      auto hilite = state.hilite;
      auto enabled = state.enabled;
      auto body_color = _body_color;

      // Draw the body
      if (value)
      {
         body_color = body_color.opacity(0.5);
         bounds = bounds.move(1, 1);
      }
      else
      {
         body_color = body_color.level(0.9);
      }
      draw_button_base(ctx, bounds, body_color, enabled, theme.button_corner_radius);

      // Adjust the font size
      auto font = theme.label_font;
      font = font.size(font._size * _size);

      // Measure text and icon
      auto text_size = measure_text(ctx.canvas, _label.c_str(), font.size(font._size * _size));
      auto icon_space = (_size * theme.icon_font._size) + 8;
      auto size = text_size;

      if (_icon_placement != icon_none)
         size.x += icon_space;

      auto text_c = enabled?
         theme.label_font_color :
         theme.label_font_color.opacity(theme.label_font_color.alpha * theme.disabled_opacity)
         ;

      if (hilite && enabled)
         text_c = text_c.level(1.5);

      auto mid_x = bounds.left + (bounds.width() / 2);
      auto mid_y = bounds.top + (bounds.height() / 2);

      // Draw label
      point text_pos;

      switch (_align)
      {
         case align_left:
            text_pos = point{bounds.left + margin.left + icon_space, mid_y};
            break;
         case align_center:
            {
               float pos_x = mid_x - (size.x/2);
               text_pos = point(pos_x + (_icon_placement == icon_left? icon_space : 0), mid_y);
            }
            break;
         case align_right:
            text_pos = point{bounds.right - (size.x + margin.right), mid_y};
            break;
      }

      cnv.fill_style(text_c);
      cnv.font(font);
      cnv.text_align(cnv.left | cnv.middle);
      cnv.fill_text(_label.c_str(), text_pos);

      // Draw icon
      if (_icon_placement != icon_none)
      {
         auto icon_pos = text_pos.move((_icon_placement == icon_left)? -icon_space : text_size.x + 8, 0);
         auto icon_font = theme.icon_font;
         cnv.font(icon_font.size(_size * icon_font._size));
         cnv.fill_text(codepoint_to_utf8(_icon_code).c_str(), icon_pos);
      }
   }
}}

